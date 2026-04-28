#include "core/storage_service.hpp"

#include "core/logger.hpp"

#if __has_include("esp_vfs_fat.h") && __has_include("esp_partition.h")
#include "esp_err.h"
#include "esp_partition.h"
#include "esp_vfs_fat.h"
#define FLINTOS_HAS_ESP_FATFS 1
#else
#define FLINTOS_HAS_ESP_FATFS 0
#endif

namespace flintos {

namespace {

constexpr const char* StorageRoot = "";

} // namespace

void StorageService::initialize() {
    Logger::info("Initializing storage service");

#if FLINTOS_HAS_ESP_FATFS
    if (!mounted_) {
        const esp_vfs_fat_mount_config_t mountConfig = {
            .format_if_mount_failed = false,
            .max_files = 8,
            .allocation_unit_size = 4096,
            .disk_status_check_enable = false,
            .use_one_fat = false,
        };
        esp_err_t err = esp_vfs_fat_spiflash_mount_ro(StorageRoot, "storage", &mountConfig);
        if (err != ESP_OK) {
            lastError_ = "failed to mount storage partition";
            Logger::error(lastError_);
            initialized_ = false;
            mounted_ = false;
            return;
        }
        mounted_ = true;
    }
#endif

    initialized_ = true;
    lastError_ = nullptr;
}

bool StorageService::isInitialized() const {
    return initialized_;
}

bool StorageService::exists(const char* path) const {
    return initialized_ && path != nullptr && path[0] != '\0';
}

const char* StorageService::rootPath() const {
    return StorageRoot;
}

const char* StorageService::lastError() const {
    return lastError_;
}

} // namespace flintos
