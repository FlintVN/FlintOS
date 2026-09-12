
#ifndef __FLINTOS_HAL_TOUCH_FT6336_H
#define __FLINTOS_HAL_TOUCH_FT6336_H

#include "flintos_hal_touch.h"

class FT6336 : public HAL::Touch {
public:
    void init(void) const;
    bool read(uint16_t *x, uint16_t *y) const;
};

#endif /* __FLINTOS_HAL_TOUCH_FT6336_H */
