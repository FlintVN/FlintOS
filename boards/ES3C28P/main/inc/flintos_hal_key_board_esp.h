
#ifndef __FLINTOS_HAL_KEY_BOARD_ESP_H
#define __FLINTOS_HAL_KEY_BOARD_ESP_H

#include "flintos_hal_key_board.h"

class EspKeyBoard : public HAL::KeyBoard {
public:
    void init(void) const;
    HAL::KeyState read(uint32_t *keyCode) const;
};

#endif /* __FLINTOS_HAL_KEY_BOARD_ESP_H */
