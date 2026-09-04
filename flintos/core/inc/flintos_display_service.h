
#ifndef __DISPLAY_SERVICE_H
#define __DISPLAY_SERVICE_H

#include <stdint.h>

class DisplaySrv {
public:
    typedef struct {
        struct {
            uint16_t x;
            uint16_t y;
            uint16_t width;
            uint16_t height;
        } invalid;

        uint16_t width;
        uint16_t height;
        uint8_t *buffer;
    } Surface;

    static void setBrightness(uint8_t value);
    static void present(Surface *surf);
    static void flush(void);
    static void showLogo(void);
};

#endif /* __DISPLAY_SERVICE_H */
