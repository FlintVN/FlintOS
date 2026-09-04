
#ifndef __FLINTOS_HAL_WIFI_H
#define __FLINTOS_HAL_WIFI_H

#include <stdint.h>

namespace HAL {
    class WiFi {
    public:
        typedef struct {
            uint8_t mac[6];
            uint8_t ssid[32];
            int8_t rssi;
            uint8_t authmode;
        } ApRecordType;

        virtual void init(void) const = 0;
        virtual bool connect(const char *ssid, uint8_t ssidLen, const char *password, uint8_t passLen, uint8_t authMode) const = 0;
        virtual void disconnect(void) const = 0;
        virtual bool isConnected(void) const = 0;
        virtual bool getAPinfo(ApRecordType *apInfo) const = 0;
        virtual bool softAP(const char *ssid, uint8_t ssidLen, const char *password, uint8_t passLen, uint8_t authMode, uint8_t channel, uint8_t maxConnection) const = 0;
        virtual void softAPDisconnect(void) const = 0;
        virtual bool startScan(bool blocked) const = 0;
        virtual int32_t getScanAPCount(void) const = 0;
        virtual bool getScanAPInfo(ApRecordType *apInfo) const = 0;
        virtual void scanClear(void) const = 0;
        virtual void stopScan(void) const = 0;
    };
}

#endif /* __FLINTOS_HAL_WIFI_H */
