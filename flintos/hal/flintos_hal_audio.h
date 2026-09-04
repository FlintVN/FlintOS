
#ifndef __FLINTOS_HAL_AUDIO_H
#define __FLINTOS_HAL_AUDIO_H

#include <stdint.h>

namespace HAL {
    class Audio {
    public:
        virtual void init(void) const = 0;
        virtual uint32_t write(uint8_t *data, uint32_t length) const = 0;
        virtual void setVolumn(uint8_t value) const = 0;
    };
}

#endif /* __FLINTOS_HAL_AUDIO_H */
