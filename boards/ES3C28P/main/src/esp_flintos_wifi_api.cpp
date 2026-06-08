
#include "flintos_devices.h"

bool FDev::WiFi::isSupported(void) {
    // TODO
    return false;
}

bool FDev::WiFi::connect(const char *ssid, uint8_t ssidLen, const char *password, uint8_t passLen, uint8_t authMode) {
    // TODO
    return false;
}

void FDev::WiFi::disconnect(void) {
    
}

bool FDev::WiFi::isConnected(void) {
    // TODO
    return false;
}

bool FDev::WiFi::getAPinfo(FDev::WiFi::ApRecordType *apInfo) {
    // TODO
    return false;
}

bool FDev::WiFi::softAP(const char *ssid, uint8_t ssidLen, const char *password, uint8_t passLen, uint8_t authMode, uint8_t channel, uint8_t maxConnection) {
    // TODO
    return false;
}

void FDev::WiFi::softAPDisconnect(void) {

}

bool FDev::WiFi::startScan(bool blocked) {
    // TODO
    return false;
}

int32_t FDev::WiFi::getScanAPCount(void) {
    // TODO
    return -1;
}

bool FDev::WiFi::getScanAPInfo(FDev::WiFi::ApRecordType *apInfo) {
    // TODO
    return false;
}

void FDev::WiFi::scanClear(void) {

}

void FDev::WiFi::stopScan(void) {

}
