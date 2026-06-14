
#include <string.h>
#include "esp_wifi.h"
#include "flintos_devices.h"

bool FDev::WiFi::isSupported(void) {
    return true;
}

bool FDev::WiFi::connect(const char *ssid, uint8_t ssidLen, const char *password, uint8_t passLen, uint8_t authMode) {
    wifi_config_t wifiConfig = {};
    memcpy(wifiConfig.sta.ssid, ssid, ssidLen);
    memcpy(wifiConfig.sta.password, password, passLen);
    wifiConfig.sta.threshold.authmode = (wifi_auth_mode_t)authMode;
    wifiConfig.sta.pmf_cfg.capable = true;
    wifiConfig.sta.pmf_cfg.required = false;

    esp_err_t ret = esp_wifi_set_config(WIFI_IF_STA, &wifiConfig);
    if(ret == ESP_OK)
        ret = esp_wifi_start();
    if(ret == ESP_OK)
        ret = esp_wifi_connect();

    return ret == ESP_OK;
}

void FDev::WiFi::disconnect(void) {
    esp_wifi_disconnect();
}

bool FDev::WiFi::isConnected(void) {
    wifi_ap_record_t ap_info;
    if(esp_wifi_sta_get_ap_info(&ap_info) != ESP_OK)
        return false;

    esp_netif_t *netif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
    esp_netif_ip_info_t ip_info;
    esp_netif_get_ip_info(netif, &ip_info);
    return ip_info.ip.addr ? true : false;
}

bool FDev::WiFi::getAPinfo(FDev::WiFi::ApRecordType *apInfo) {
    wifi_ap_record_t apRecord;
    esp_err_t ret = esp_wifi_sta_get_ap_info(&apRecord);
    if(ret != ESP_OK) return false;
    memcpy(apInfo->mac, apRecord.bssid, 6);
    strncpy((char *)apInfo->ssid, (char *)apRecord.ssid, 32);
    apInfo->rssi = apRecord.rssi;
    apInfo->authmode = (uint8_t)apRecord.authmode;
    return true;
}

bool FDev::WiFi::softAP(const char *ssid, uint8_t ssidLen, const char *password, uint8_t passLen, uint8_t authMode, uint8_t channel, uint8_t maxConnection) {
    wifi_config_t wifiConfig = {};
    wifiConfig.ap.ssid_len = ssidLen;
    wifiConfig.ap.channel = channel;
    wifiConfig.ap.max_connection = maxConnection;
    wifiConfig.ap.authmode = (wifi_auth_mode_t)authMode;
    wifiConfig.ap.pmf_cfg.required = false;
    memcpy(wifiConfig.ap.ssid, ssid, ssidLen);
    memcpy(wifiConfig.ap.password, password, passLen);

    esp_err_t ret = esp_wifi_set_mode(WIFI_MODE_APSTA);
    if(ret == ESP_OK)
        ret = esp_wifi_set_config(WIFI_IF_AP, &wifiConfig);
    if(ret == ESP_OK)
        ret = esp_wifi_start();

    return ret == ESP_OK;
}

void FDev::WiFi::softAPDisconnect(void) {
    esp_wifi_set_mode(WIFI_MODE_STA);
}

bool FDev::WiFi::startScan(bool blocked) {
    return esp_wifi_scan_start(NULL, blocked ? true : false) == ESP_OK;
}

int32_t FDev::WiFi::getScanAPCount(void) {
    uint16_t count = 0;
    esp_err_t ret = esp_wifi_scan_get_ap_num(&count);
    return (ret == ESP_OK) ? count : -1;
}

bool FDev::WiFi::getScanAPInfo(FDev::WiFi::ApRecordType *apInfo) {
    wifi_ap_record_t apRecord;
    esp_err_t ret = esp_wifi_scan_get_ap_record(&apRecord);
    if(ret != ESP_OK) return false;
    memcpy(apInfo->mac, apRecord.bssid, 6);
    strncpy((char *)apInfo->ssid, (char *)apRecord.ssid, 32);
    apInfo->rssi = apRecord.rssi;
    apInfo->authmode = (uint8_t)apRecord.authmode;
    return true;
}

void FDev::WiFi::scanClear(void) {
    esp_wifi_clear_ap_list();
}

void FDev::WiFi::stopScan(void) {
    esp_wifi_scan_stop();
}
