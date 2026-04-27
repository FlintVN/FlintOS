#pragma once

namespace flintos {

class NetworkDriver {
public:
    void initialize();
    bool isInitialized() const;
    bool isWifiEnabled() const;
    bool connectWifi(const char* ssid);

private:
    bool initialized_ = false;
    bool wifiEnabled_ = false;
};

} // namespace flintos
