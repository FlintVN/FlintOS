
#ifndef __ILI9341_LCD_DRIVER_H
#define __ILI9341_LCD_DRIVER_H

#include <stdint.h>

#define LCD_CS_PIN              10
#define LCD_MOSI_PIN            11
#define LCD_SCK_PIN             12
#define LCD_MISO_PIN            13
#define LCD_LED_PIN             45
#define LCD_DC_PIN              46

void LCD_Init(void);
void LCD_Brightness(uint8_t value);
bool LCD_Write(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t *data);

#endif /* __ILI9341_LCD_DRIVER_H */
