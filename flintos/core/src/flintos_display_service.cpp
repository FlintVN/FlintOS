
#include <stddef.h>
#include <string.h>
#include <stdatomic.h>
#include "flintos_devices.h"
#include "flint_system_api.h"
#include "flint_file_reader.h"
#include "flintos_default_conf.h"
#include "flintos_display_service.h"

typedef struct __attribute__((packed)) {
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
} BitmapFileHeader;

typedef struct __attribute__((packed)) {
    uint32_t biSize;
    int32_t biWidth;
    int32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t biXPelsPerMeter;
    int32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
} BitmapInfoHeader;

static atomic_flag displayLocked = ATOMIC_FLAG_INIT;
static uint16_t ymin = DISPLAY_HEIGHT - 1;
static uint16_t ymax = 0;
alignas(2) static uint8_t displayBuff[DISPLAY_WIDTH * DISPLAY_HEIGHT * 2];

static void Display_Lock(void) {
    while(atomic_flag_test_and_set_explicit(&displayLocked, memory_order_acquire))
        FlintAPI::Thread::yield();
}

static void Display_Unlock(void) {
    atomic_flag_clear_explicit(&displayLocked, memory_order_release);
}

static void Display_Clear(void) {
    memset(displayBuff, 0, sizeof(displayBuff));
}

static void UpdateRequest(uint16_t y, uint16_t h) {
    uint16_t y2 = y + h - 1;
    Display_Lock();
    if(y < ymin) ymin = y;
    if(y2 > ymax) ymax = y2;
    Display_Unlock();
}

static void ConvertToRgb565(uint16_t *data, uint32_t length) {
    for(uint32_t i = 0; i < length; i++) {
        uint16_t tmp = ((data[i] << 1) & 0xFFC0) | (data[i] & 0x1F);
        data[i] = __builtin_bswap16(tmp);
    }
}

static bool ReadRgb555(FileReader *reader, BitmapInfoHeader *infoHeader) {
    int32_t bitmapWidth = infoHeader->biWidth;
    int32_t bitmapHeight = infoHeader->biHeight;

    if(bitmapWidth <= 0 || bitmapHeight == 0) return false;

    uint32_t width = bitmapWidth;
    uint32_t height = bitmapHeight > 0 ? bitmapHeight : -bitmapHeight;
    if(width > DISPLAY_WIDTH || height > DISPLAY_HEIGHT) return false;

    uint16_t x = (DISPLAY_WIDTH - width) / 2;
    uint16_t y = (DISPLAY_HEIGHT - height) / 2;
    uint32_t rowSize = width * 2;

    for(uint32_t i = 0; i < height; i++) {
        uint32_t row = bitmapHeight > 0 ? height - 1 - i : i;
        uint16_t *buff = &((uint16_t *)displayBuff)[(row + y) * DISPLAY_WIDTH + x];
        if(reader->read(buff, rowSize) != rowSize)
            return false;
        ConvertToRgb565(buff, width);
    }
    return true;
}

void DisplaySrv::setBrightness(uint8_t value) {
    FDev::Display::brightness(value);
}

void DisplaySrv::write(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t *data) {
    if(w == DISPLAY_WIDTH)
        memcpy(&displayBuff[(y * DISPLAY_WIDTH + x) * 2], data, w * h * 2);
    else {
        uint32_t rowSz = w * 2;
        for(uint32_t i = 0; i < h; i++) {
            memcpy(&displayBuff[((i + y) * DISPLAY_WIDTH + x) * 2], data, rowSz);
            data += rowSz;
        }
    }
    UpdateRequest(y, h);
}

void DisplaySrv::update(void) {
    if(ymin > ymax) return;

    Display_Lock();
    uint16_t y1 = ymin;
    uint16_t y2 = ymax;
    Display_Unlock();

    FDev::Display::write(0, y1, DISPLAY_WIDTH, y2 - y1 + 1, &displayBuff[y1 * DISPLAY_WIDTH * 2]);
    if(ymin == y1 && ymax == y2) {
        Display_Lock();
        if(ymin == y1 && ymax == y2) {  /* Double-check to ensure there are no changes. */
            ymin = DISPLAY_HEIGHT - 1;
            ymax = 0;
        }
        Display_Unlock();
    }
}

void DisplaySrv::showLogo(void) {
    Display_Clear();

    FileReader reader(NULL, "/sys/icons/flint_64x64.bmp");
    if(!reader.open()) return;

    do {
        BitmapFileHeader fileHeader;
        BitmapInfoHeader infoHeader;

        if(reader.read(&fileHeader, sizeof(BitmapFileHeader)) != sizeof(BitmapFileHeader)) break;
        if(reader.read(&infoHeader, sizeof(BitmapInfoHeader)) != sizeof(BitmapInfoHeader)) break;

        if(infoHeader.biCompression != 0 || infoHeader.biBitCount != 16 || !reader.seek(fileHeader.bfOffBits)) break;

        if(!ReadRgb555(&reader, &infoHeader)) break;
    } while(0);

    reader.close();
    UpdateRequest(0, DISPLAY_HEIGHT);
    update();
}
