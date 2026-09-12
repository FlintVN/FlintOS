
#ifndef __FLINTOS_HAL_TOUCH_H
#define __FLINTOS_HAL_TOUCH_H

#include <stdint.h>

namespace HAL {
    class Touch {
    public:
        virtual void init(void) const = 0;
        virtual bool read(uint16_t *x, uint16_t *y) const = 0;
    };
}

#endif /* __FLINTOS_HAL_TOUCH_H */
