
#include <stddef.h>
#include <string.h>
#include <stdatomic.h>
#include "flintos_screen.h"
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

alignas(2) static uint8_t screenBuff[SCREEN_WIDTH * SCREEN_HEIGHT * 2];
static atomic_bool requireUpdate = false;

static void Screen_Clear(void) {
    memset(screenBuff, 0, sizeof(screenBuff));
}

static void ConvertToRgb565(uint16_t *data, uint32_t length) {
    for(uint32_t i = 0; i < length; i++) {
        uint16_t tmp = ((data[i] << 1) & 0xFFC0) | (data[i] & 0x1F);
        data[i] = __builtin_bswap16(tmp);
    }
}

static bool ReadRgb555(FileReader *reader, BitmapInfoHeader *infoHeader) {
    int32_t w = infoHeader->biWidth;
    int32_t h = infoHeader->biHeight;
    uint16_t x = (SCREEN_WIDTH - w) / 2;
    uint16_t y = (SCREEN_HEIGHT - std::abs(h)) / 2;
    uint32_t rowSize = w * 2;
    if(h > 0) for(uint32_t i = 0; i < h; i++) {
        uint16_t *buff = &((uint16_t *)screenBuff)[((h - 1 - i) + y) * SCREEN_WIDTH + x];
        if(reader->read(buff, rowSize) != rowSize)
            return false;
        ConvertToRgb565(buff, w);
    }
    else for(uint32_t i = 0; i < h; i++) {
        uint16_t *buff = &((uint16_t *)screenBuff)[(i + y) * SCREEN_WIDTH + x];
        if(reader->read(buff, rowSize) != rowSize)
            return false;
        ConvertToRgb565(buff, w);
    }
    return true;
}

void FosScreen::setBrightness(uint8_t value) {
    FDev::Screen::brightness(value);
}

void FosScreen::write(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t *data) {
    if(w == SCREEN_WIDTH)
        memcpy(&screenBuff[(y * SCREEN_WIDTH + x) * 2], data, w * h * 2);
    else {
        uint32_t rowSz = w * 2;
        for(uint32_t i = 0; i < h; i++) {
            memcpy(&screenBuff[((i + y) * SCREEN_WIDTH + x) * 2], data, rowSz);
            data += rowSz;
        }
    }
    requireUpdate.store(true);
}

bool FosScreen::update(void) {
    bool expected = true;
    if(requireUpdate.compare_exchange_strong(expected, false))
        return FDev::Screen::write(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, screenBuff);
    else
        return true;
}

void FosScreen::showLogo(void) {
    Screen_Clear();

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
    requireUpdate.store(true);
}
