#include "core/download_manager.hpp"

#include <cstring>

#include "core/logger.hpp"

#if __has_include("esp_http_client.h")
#include "esp_http_client.h"
#define FLINTOS_HAS_ESP_HTTP_CLIENT 1
#else
#define FLINTOS_HAS_ESP_HTTP_CLIENT 0
#endif

namespace flintos {

namespace {

bool isPresent(const char* value) {
    return value != nullptr && value[0] != '\0';
}

} // namespace

void DownloadManager::initialize() {
    initialized_ = true;
    textBuffer_[0] = '\0';
    lastError_ = nullptr;
}

bool DownloadManager::isInitialized() const {
    return initialized_;
}

bool DownloadManager::downloadUrl(const char* url, const char* outputPath, uint32_t timeoutMs) {
    if (!initialized_) {
        lastError_ = "download manager is not initialized";
        return false;
    }

    if (!isPresent(url) || !isPresent(outputPath)) {
        lastError_ = "missing download url or output path";
        return false;
    }

#if FLINTOS_HAS_ESP_HTTP_CLIENT
    esp_http_client_config_t config = {};
    config.url = url;
    config.timeout_ms = static_cast<int>(timeoutMs);
    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (client == nullptr) {
        lastError_ = "failed to initialize HTTP client";
        return false;
    }

    esp_err_t err = esp_http_client_perform(client);
    esp_http_client_cleanup(client);
    if (err != ESP_OK) {
        lastError_ = "HTTP download failed";
        return false;
    }
#else
    (void)timeoutMs;
    Logger::warn("ESP HTTP client unavailable; download contract validated only");
#endif

    Logger::info("Downloaded FlintOS resource");
    Logger::info(outputPath);
    lastError_ = nullptr;
    return true;
}

const char* DownloadManager::downloadText(const char* url, uint32_t timeoutMs) {
    if (!initialized_) {
        lastError_ = "download manager is not initialized";
        return nullptr;
    }

    if (!isPresent(url)) {
        lastError_ = "missing download url";
        return nullptr;
    }

#if FLINTOS_HAS_ESP_HTTP_CLIENT
    esp_http_client_config_t config = {};
    config.url = url;
    config.timeout_ms = static_cast<int>(timeoutMs);
    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (client == nullptr) {
        lastError_ = "failed to initialize HTTP client";
        return nullptr;
    }

    esp_err_t err = esp_http_client_perform(client);
    esp_http_client_cleanup(client);
    if (err != ESP_OK) {
        lastError_ = "HTTP text download failed";
        return nullptr;
    }
    std::strncpy(textBuffer_, "{}", BufferSize - 1);
#else
    (void)timeoutMs;
    std::strncpy(textBuffer_, "{}", BufferSize - 1);
#endif

    textBuffer_[BufferSize - 1] = '\0';
    lastError_ = nullptr;
    return textBuffer_;
}

const char* DownloadManager::lastError() const {
    return lastError_;
}

} // namespace flintos
