
#include <algorithm>
#include "driver/gpio.h"
#include "flintos_default_conf.h"
#include "flintos_hal_key_board_esp.h"

#define HOME_BUTTON     GPIO_NUM_0

void EspKeyBoard::init(void) const {
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.pin_bit_mask = 1ULL << HOME_BUTTON;
    io_conf.mode = GPIO_MODE_INPUT,
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf);
}

HAL::KeyState EspKeyBoard::read(uint32_t *keyCode) const {
    static HAL::KeyState oldState = HAL::KEY_RELEASED;
    HAL::KeyState state = gpio_get_level(HOME_BUTTON) ? HAL::KEY_RELEASED : HAL::KEY_PRESSED;
    if (oldState != state) {
        oldState = state;
        *keyCode = HAL::KEYCODE_HOME;
        return state;
    }
    return HAL::KEY_NONE;
}
