
#ifndef __DISPLAY_SERVICE_H
#define __DISPLAY_SERVICE_H

#include <stdint.h>

class DisplaySrv {
public:
    static void setBrightness(uint8_t value);
    static void write(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t *data);
    static void update(void);
    static void showLogo(void);
};

#endif /* __DISPLAY_SERVICE_H */
