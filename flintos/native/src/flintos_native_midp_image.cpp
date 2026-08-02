
#include "flintos_native_midp_image.h"
#include "flint_array_object.h"
#include <stdlib.h>
#include <string.h>

/* ESP32-S3 ROM miniz: raw deflate/zlib inflate into a caller-provided buffer (no internal
 * allocation, so no ROM-vs-app allocator mismatch). Linked from esp32s3.rom.ld. */
extern "C" size_t tinfl_decompress_mem_to_mem(void *pOut_buf, size_t out_buf_len, const void *pSrc_buf, size_t src_buf_len, int flags);

#define TINFL_FLAG_PARSE_ZLIB_HEADER                1
#define TINFL_FLAG_USING_NON_WRAPPING_OUTPUT_BUF    4
#define TINFL_FAILED                                ((size_t) - 1)

static inline uint32_t be32(const uint8_t *p) {
    return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) | ((uint32_t)p[2] << 8) | p[3];
}

static inline int paeth(int a, int b, int c) {
    int p = a + b - c;
    int pa = p > a ? p - a : a - p;
    int pb = p > b ? p - b : b - p;
    int pc = p > c ? p - c : c - p;
    if(pa <= pb && pa <= pc) return a;
    if(pb <= pc) return b;
    return c;
}

/* Flint BGR-packed big-endian RGB565 (matches Image.pack565 / the framebuffer). */
static inline void put565(uint8_t *d, int r, int g, int b) {
    uint16_t v = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
    d[0] = (uint8_t)(v >> 8);
    d[1] = (uint8_t)v;
}

jbyteArray NativeMidpImage_PngDecode(FNIEnv *env, jbyteArray inArr, jint off, jint len, jintArray whObj) {
    if(inArr == NULL || inArr == NULL) return NULL;
    if(off < 0 || len < 8 || (uint32_t)(off + len) > inArr->getLength()) return NULL;
    const uint8_t *png = (const uint8_t *)inArr->getData() + off;
    if(png[0] != 0x89 || png[1] != 0x50 || png[2] != 0x4E || png[3] != 0x47) return NULL;

    int W = 0, H = 0, bitDepth = 0, colorType = 0, interlace = 0;
    uint8_t palette[256 * 3];
    uint8_t trns[256];
    int palCount = 0, trnsCount = 0;
    uint8_t *idat = NULL;
    size_t idatLen = 0, idatCap = 0;
    bool haveIHDR = false;

    uint32_t pos = 8;
    while(pos + 12 <= (uint32_t)len) {
        uint32_t clen = be32(png + pos);
        const uint8_t *ctype = png + pos + 4;
        const uint8_t *cdata = png + pos + 8;
        if((uint64_t)pos + 12 + clen > (uint32_t)len) break;
        if(!memcmp(ctype, "IHDR", 4) && clen >= 13) {
            W = (int)be32(cdata); H = (int)be32(cdata + 4);
            bitDepth = cdata[8]; colorType = cdata[9]; interlace = cdata[12];
            haveIHDR = true;
        } else if(!memcmp(ctype, "PLTE", 4)) {
            palCount = clen / 3; if(palCount > 256) palCount = 256;
            memcpy(palette, cdata, (size_t)palCount * 3);
        } else if(!memcmp(ctype, "tRNS", 4)) {
            trnsCount = clen > 256 ? 256 : (int)clen;
            memcpy(trns, cdata, (size_t)trnsCount);
        } else if(!memcmp(ctype, "IDAT", 4)) {
            if(idatLen + clen > idatCap) {
                idatCap = (idatLen + clen) * 2 + 1024;
                uint8_t *n = (uint8_t *)realloc(idat, idatCap);
                if(!n) { free(idat); return NULL; }
                idat = n;
            }
            memcpy(idat + idatLen, cdata, clen); idatLen += clen;
        } else if(!memcmp(ctype, "IEND", 4)) {
            break;
        }
        pos += 12 + clen;
    }
    if(!haveIHDR || W <= 0 || H <= 0 || W > 2048 || H > 2048 || interlace != 0 || idatLen == 0) {
        free(idat); return NULL;
    }

    int channels;
    switch(colorType) {
        case 0: channels = 1; break;   /* gray        */
        case 2: channels = 3; break;   /* rgb         */
        case 3: channels = 1; break;   /* palette     */
        case 4: channels = 2; break;   /* gray+alpha  */
        case 6: channels = 4; break;   /* rgba        */
        default: free(idat); return NULL;
    }
    bool subByte = (colorType == 3 && (bitDepth == 1 || bitDepth == 2 || bitDepth == 4));
    if(bitDepth != 8 && bitDepth != 16 && !subByte) { free(idat); return NULL; }

    int bitsPerPixel = channels * bitDepth;
    int rowBytes = (W * bitsPerPixel + 7) / 8;
    int bpp = (bitsPerPixel + 7) / 8; if(bpp < 1) bpp = 1;
    size_t expanded = (size_t)(rowBytes + 1) * H;          /* +1 filter byte / row */

    uint8_t *raw = (uint8_t *)malloc(expanded);
    if(!raw) { free(idat); return NULL; }
    size_t got = tinfl_decompress_mem_to_mem(raw, expanded, idat, idatLen,
        TINFL_FLAG_PARSE_ZLIB_HEADER | TINFL_FLAG_USING_NON_WRAPPING_OUTPUT_BUF);
    free(idat);
    if(got == TINFL_FAILED || got < expanded) { free(raw); return NULL; }

    /* Unfilter scanlines into a packed (rowBytes * H) buffer. */
    uint8_t *img = (uint8_t *)malloc((size_t)rowBytes * H);
    if(!img) { free(raw); return NULL; }
    uint8_t *prev = NULL;
    for(int y = 0; y < H; y++) {
        uint8_t ft = raw[(size_t)y * (rowBytes + 1)];
        const uint8_t *src = raw + (size_t)y * (rowBytes + 1) + 1;
        uint8_t *cur = img + (size_t)y * rowBytes;
        for(int x = 0; x < rowBytes; x++) {
            int a = (x >= bpp) ? cur[x - bpp] : 0;
            int b = prev ? prev[x] : 0;
            int c = (prev && x >= bpp) ? prev[x - bpp] : 0;
            int val = src[x];
            switch(ft) {
                case 1: val += a; break;
                case 2: val += b; break;
                case 3: val += (a + b) >> 1; break;
                case 4: val += paeth(a, b, c); break;
                default: break;            /* 0 = None */
            }
            cur[x] = (uint8_t)val;
        }
        prev = cur;
    }
    free(raw);

    bool hasAlpha = (colorType == 4 || colorType == 6) ||
                    ((colorType == 3 || colorType == 0) && trnsCount > 0);
    size_t rgbBytes = (size_t)W * H * 2;
    size_t aBytes = hasAlpha ? (((size_t)W * H + 1) >> 1) : 0;

    jbyteArray out = env->newByteArray((uint32_t)(rgbBytes + aBytes));   /* last Flint alloc */
    if(out == NULL) { free(img); return NULL; }
    uint8_t *od = (uint8_t *)out->getData();
    uint8_t *ap = od + rgbBytes;
    if(hasAlpha) memset(ap, 0, aBytes);

    int s = (bitDepth == 16) ? 2 : 1;       /* sample stride: take the high byte of 16-bit */
    for(int y = 0; y < H; y++) {
        const uint8_t *row = img + (size_t)y * rowBytes;
        for(int x = 0; x < W; x++) {
            int r, g, b, al = 255;
            if(colorType == 2) {
                const uint8_t *p = row + (size_t)x * 3 * s;
                r = p[0]; g = p[s]; b = p[2 * s];
            } else if(colorType == 6) {
                const uint8_t *p = row + (size_t)x * 4 * s;
                r = p[0]; g = p[s]; b = p[2 * s]; al = p[3 * s];
            } else if(colorType == 0) {
                int gray = row[(size_t)x * s];
                r = g = b = gray;
                if(trnsCount >= 2 && gray == trns[1]) al = 0;   /* gray key (8-bit) */
            } else if(colorType == 4) {
                const uint8_t *p = row + (size_t)x * 2 * s;
                r = g = b = p[0]; al = p[s];
            } else {                                            /* colorType 3: palette */
                int idx;
                if(bitDepth == 8) {
                    idx = row[x];
                } else {
                    int perByte = 8 / bitDepth;
                    int shift = 8 - bitDepth * ((x % perByte) + 1);
                    idx = (row[x / perByte] >> shift) & ((1 << bitDepth) - 1);
                }
                if(idx < palCount) { r = palette[idx * 3]; g = palette[idx * 3 + 1]; b = palette[idx * 3 + 2]; }
                else               { r = g = b = 0; }
                if(idx < trnsCount) al = trns[idx];
            }
            int pi = y * W + x;
            put565(od + (size_t)pi * 2, r, g, b);
            if(hasAlpha) {
                int a4 = al >> 4;
                int ai = pi >> 1;
                if(pi & 1) ap[ai] = (uint8_t)((ap[ai] & 0x0F) | (a4 << 4));
                else       ap[ai] = (uint8_t)((ap[ai] & 0xF0) | a4);
            }
        }
    }
    free(img);

    int32_t *whp = whObj->getData();
    whp[0] = W; whp[1] = H; whp[2] = hasAlpha ? 3 : 2;
    return out;
}
