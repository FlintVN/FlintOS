
#ifndef __FLINTOS_HAL_DISPLAY_H
#define __FLINTOS_HAL_DISPLAY_H

#include <stdint.h>

namespace HAL {
    class Display {
    public:
        virtual void init(void) const = 0;
        virtual void write(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t *data, uint16_t stride) const = 0;
        virtual void brightness(uint8_t value) const = 0;
    };
}

#endif /* __FLINTOS_HAL_DISPLAY_H */
