#include "hal/power_manager.hpp"

#include "core/logger.hpp"

namespace flintos {

void PowerManager::initialize() {
    Logger::info("Initializing power and watchdog policy");
    initialized_ = true;
}

bool PowerManager::isInitialized() const {
    return initialized_;
}

void PowerManager::feedWatchdog() {
    if (initialized_) {
        Logger::info("Watchdog feed requested");
    }
}

const char* PowerManager::policy() const {
    return initialized_ ? "balanced" : "uninitialized";
}

} // namespace flintos
