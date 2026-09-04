
#ifndef __FLINTOS_HAL_DEVICES_H
#define __FLINTOS_HAL_DEVICES_H

#include "flintos_hal_wifi.h"
#include "flintos_hal_audio.h"
#include "flintos_hal_display.h"

namespace HAL {
    class Devices {
    public:
        static const WiFi *wifi(void);
        static const Audio *audio(void);
        static const Display *display(void);
    };
}

#endif /* __FLINTOS_HAL_DEVICES_H */
