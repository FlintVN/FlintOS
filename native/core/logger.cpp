#include "core/logger.hpp"

#include "esp_log.h"

namespace flintos {

namespace {
constexpr const char* TAG = "FlintOS";
}

void Logger::info(const char* message) {
    ESP_LOGI(TAG, "%s", message);
}

void Logger::warn(const char* message) {
    ESP_LOGW(TAG, "%s", message);
}

void Logger::error(const char* message) {
    ESP_LOGE(TAG, "%s", message);
}

} // namespace flintos
