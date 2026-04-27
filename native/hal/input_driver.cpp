#include "hal/input_driver.hpp"

#include "core/logger.hpp"

namespace flintos {

void InputDriver::initialize() {
    Logger::info("Initializing input HAL");
    initialized_ = true;
}

bool InputDriver::isInitialized() const {
    return initialized_;
}

bool InputDriver::hasPendingEvent() const {
    return false;
}

const char* InputDriver::readEventType() const {
    return initialized_ ? "none" : "uninitialized";
}

} // namespace flintos
