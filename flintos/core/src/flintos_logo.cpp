
#include <stddef.h>
#include "flintos_logo.h"
#include "flintos_devices.h"
#include "flint_system_api.h"
#include "flint_file_reader.h"
#include "flintos_default_conf.h"

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

static void ConvertToRgb565(uint16_t *data, uint32_t length) {
    for(uint32_t i = 0; i < length; i++) {
        uint16_t tmp = ((data[i] << 1) & 0xFFC0) | (data[i] & 0x1F);
        data[i] = __builtin_bswap16(tmp);
    }
}

static bool ReadRgb555(FileReader *reader, int32_t w, int32_t h, uint8_t *buff) {
    uint32_t imgSize = w * (h < 0 ? -h : h) * 2;
    if(h > 0) {
        uint32_t rowSize = w * 2;
        for(uint32_t i = 0; i < h; i++) {
            uint32_t y = h - 1 - i;
            if(reader->read(&buff[y * rowSize], rowSize) != rowSize)
                return false;
        }
    }
    else {
        if(reader->read(buff, imgSize) != imgSize)
            return false;
    }
    ConvertToRgb565((uint16_t *)buff, imgSize / 2);
    return true;
}

void Logo_Show(void) {
    uint8_t *data = NULL;
    FileReader reader(NULL, "/sys/icons/flint_64x64.bmp");

    if(!reader.open()) return;

    do {
        BitmapFileHeader fileHeader;
        BitmapInfoHeader infoHeader;

        if(reader.read(&fileHeader, sizeof(BitmapFileHeader)) != sizeof(BitmapFileHeader)) break;
        if(reader.read(&infoHeader, sizeof(BitmapInfoHeader)) != sizeof(BitmapInfoHeader)) break;

        if(infoHeader.biCompression != 0 || infoHeader.biBitCount != 16 || !reader.seek(fileHeader.bfOffBits)) break;

        data = (uint8_t *)FlintAPI::System::malloc(infoHeader.biSizeImage);
        if(data == NULL) break;

        int32_t w = infoHeader.biWidth;
        int32_t h = infoHeader.biHeight;

        if(!ReadRgb555(&reader, w, h, data)) break;

        if(h < 0) h = -h;
        uint16_t x = (SCREEN_WIDTH - w) / 2;
        uint32_t y = (SCREEN_HEIGHT - h) / 2;
        FDev::Screen::writeSync(x, y, w, h, data);
    } while(0);

    reader.close();
    FlintAPI::Thread::sleep(10);
    if(data != NULL)
        FlintAPI::System::free(data);
}
