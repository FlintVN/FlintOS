#include "hal/network_driver.hpp"

#include "core/logger.hpp"

namespace flintos {

void NetworkDriver::initialize() {
    Logger::info("Initializing network HAL");
    initialized_ = true;
}

bool NetworkDriver::isInitialized() const {
    return initialized_;
}

bool NetworkDriver::isWifiEnabled() const {
    return wifiEnabled_;
}

bool NetworkDriver::connectWifi(const char* ssid) {
    if (!initialized_ || ssid == nullptr || ssid[0] == '\0') {
        return false;
    }

    Logger::info("Wi-Fi connect requested");
    wifiEnabled_ = true;
    return true;
}

} // namespace flintos
