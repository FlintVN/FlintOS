#include "hal/display_driver.hpp"

#include "core/logger.hpp"

namespace flintos {

void DisplayDriver::clear() {
    Logger::info("Display clear requested");
}

void DisplayDriver::drawText(const char* text, int x, int y) {
    (void)x;
    (void)y;
    if (text == nullptr) {
        Logger::warn("Display drawText ignored null text");
        return;
    }

    Logger::info(text);
}

} // namespace flintos
