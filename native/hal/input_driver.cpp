#include "hal/input_driver.hpp"

#include "core/logger.hpp"
#include "driver/gpio.h"

namespace flintos {
namespace {

constexpr gpio_num_t BootButtonGpio = GPIO_NUM_0;

} // namespace

void InputDriver::initialize() {
    gpio_config_t config = {};
    config.pin_bit_mask = 1ULL << BootButtonGpio;
    config.mode = GPIO_MODE_INPUT;
    config.pull_up_en = GPIO_PULLUP_ENABLE;
    config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    config.intr_type = GPIO_INTR_DISABLE;
    if (gpio_config(&config) != ESP_OK) {
        Logger::warn("BOOT0 input init failed");
        initialized_ = false;
        return;
    }

    Logger::info("Initializing input HAL BOOT0=GPIO0");
    initialized_ = true;
}

bool InputDriver::isInitialized() const {
    return initialized_;
}

bool InputDriver::hasPendingEvent() const {
    return isBootButtonPressed();
}

const char* InputDriver::readEventType() const {
    if (!initialized_) {
        return "uninitialized";
    }
    return isBootButtonPressed() ? "boot0" : "none";
}

bool InputDriver::isBootButtonPressed() const {
    return initialized_ && gpio_get_level(BootButtonGpio) == 0;
}

} // namespace flintos
