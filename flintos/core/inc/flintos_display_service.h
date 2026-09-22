
#ifndef __DISPLAY_SERVICE_H
#define __DISPLAY_SERVICE_H

#include <stdint.h>

class DisplaySrv {
public:
    typedef struct {
        struct {
            int32_t x;
            int32_t y;
            int32_t width;
            int32_t height;
        } invalid;

        int32_t width;
        int32_t height;
        uint8_t *buffer;
    } Surface;

    static void mainTask(void);
    static void setBrightness(uint8_t value);
    static void present(Surface *surf);
};

#endif /* __DISPLAY_SERVICE_H */
