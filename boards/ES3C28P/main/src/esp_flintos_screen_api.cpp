
#include "flintos_devices.h"
#include "ili9341_lcd_driver.h"

void FDev::Screen::writeSync(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t *data) {
    LCD_Write(x, y, w, h, data, false);
}

void FDev::Screen::writeAsync(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t *data) {
    LCD_Write(x, y, w, h, data, true);
}

void FDev::Screen::brightness(uint8_t value) {
    LCD_Brightness(value);
}
