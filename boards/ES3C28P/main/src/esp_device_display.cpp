
#include "flintos_devices.h"
#include "ili9341_lcd_driver.h"

bool FDev::Display::write(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t *data) {
    return LCD_Write(x, y, w, h, data);
}

void FDev::Display::brightness(uint8_t value) {
    LCD_Brightness(value);
}
