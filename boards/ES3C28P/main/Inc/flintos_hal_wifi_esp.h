
#ifndef __FLINTOS_HAL_WIFI_ESP_H
#define __FLINTOS_HAL_WIFI_ESP_H

#include "flintos_hal_wifi.h"

class EspWiFi : public HAL::WiFi {
public:
    void init(void) const;
    bool connect(const char *ssid, uint8_t ssidLen, const char *password, uint8_t passLen, uint8_t authMode) const;
    void disconnect(void) const;
    bool isConnected(void) const;
    bool getAPinfo(ApRecordType *apInfo) const;
    bool softAP(const char *ssid, uint8_t ssidLen, const char *password, uint8_t passLen, uint8_t authMode, uint8_t channel, uint8_t maxConnection) const;
    void softAPDisconnect(void) const;
    bool startScan(bool blocked) const;
    int32_t getScanAPCount(void) const;
    bool getScanAPInfo(ApRecordType *apInfo) const;
    void scanClear(void) const;
    void stopScan(void) const;
};

#endif /* __FLINTOS_HAL_WIFI_ESP_H */
