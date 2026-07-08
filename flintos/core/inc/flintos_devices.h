
#ifndef __FLINTOS_API_H
#define __FLINTOS_API_H

#include <stdint.h>
namespace FDev::Display {
    void init(void);
    void write(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint8_t *data);
    void brightness(uint8_t value);
}

namespace FDev::WiFi {
    typedef struct {
        uint8_t mac[6];
        uint8_t ssid[32];
        int8_t rssi;
        uint8_t authmode;
    } ApRecordType;

    void init(void);
    bool isSupported(void);
    bool connect(const char *ssid, uint8_t ssidLen, const char *password, uint8_t passLen, uint8_t authMode);
    void disconnect(void);
    bool isConnected(void);
    bool getAPinfo(ApRecordType *apInfo);
    bool softAP(const char *ssid, uint8_t ssidLen, const char *password, uint8_t passLen, uint8_t authMode, uint8_t channel, uint8_t maxConnection);
    void softAPDisconnect(void);
    bool startScan(bool blocked);
    int32_t getScanAPCount(void);
    bool getScanAPInfo(ApRecordType *apInfo);
    void scanClear(void);
    void stopScan(void);
}

#endif /* __FLINTOS_API_H */
