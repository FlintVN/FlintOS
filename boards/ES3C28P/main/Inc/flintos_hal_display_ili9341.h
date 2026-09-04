
#ifndef __FLINTOS_HAL_DISPLAY_ILI9341_H
#define __FLINTOS_HAL_DISPLAY_ILI9341_H

#include "flintos_hal_display.h"

class ILI9341 : public HAL::Display {
public:
    void init(void) const;
    void write(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t *data, uint16_t stride) const;
    void brightness(uint8_t value) const;
};

#endif /* __FLINTOS_HAL_DISPLAY_ILI9341_H */
