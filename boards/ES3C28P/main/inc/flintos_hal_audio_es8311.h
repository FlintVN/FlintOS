
#ifndef __FLINTOS_HAL_AUDIO_ES8311_H
#define __FLINTOS_HAL_AUDIO_ES8311_H

#include "flintos_hal_audio.h"

class ES8311 : public HAL::Audio {
public:
    void init(void) const;
    uint32_t write(uint8_t *data, uint32_t length) const;
    void setVolumn(uint8_t value) const;
};

#endif /* __FLINTOS_HAL_AUDIO_ES8311_H */
