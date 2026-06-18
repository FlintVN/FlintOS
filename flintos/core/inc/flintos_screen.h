
#ifndef __FLINTOS_SCREEN_H
#define __FLINTOS_SCREEN_H

#include <stdint.h>

class FosScreen {
public:
    static void setBrightness(uint8_t value);
    static void write(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t *data);
    static bool update(void);
    static void showLogo(void);
};

#endif /* __FLINTOS_SCREEN_H */
