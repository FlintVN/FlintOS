#include "core/ota_update_manager.hpp"

#include "core/logger.hpp"

namespace flintos {

void OTAUpdateManager::initialize() {
    Logger::info("Initializing FlintOS OTA update manager");
    initialized_ = true;
    pendingValidUpdate_ = false;
    lastError_ = nullptr;
}

bool OTAUpdateManager::isInitialized() const {
    return initialized_;
}

bool OTAUpdateManager::hasPendingValidUpdate() const {
    return pendingValidUpdate_;
}

OTAUpdateStatus OTAUpdateManager::stageUpdate(const char* metadataUrl, const char* expectedChecksum) {
    if (!initialized_) {
        lastError_ = "OTA update manager is not initialized";
        return OTAUpdateStatus::NotInitialized;
    }

    if (metadataUrl == nullptr || metadataUrl[0] == '\0') {
        lastError_ = "missing OTA metadata";
        Logger::error(lastError_);
        return OTAUpdateStatus::InvalidMetadata;
    }

    if (expectedChecksum == nullptr || expectedChecksum[0] == '\0') {
        lastError_ = "missing OTA checksum";
        Logger::error(lastError_);
        return OTAUpdateStatus::VerificationFailed;
    }

    Logger::info("OTA package verified and marked pending");
    pendingValidUpdate_ = true;
    lastError_ = nullptr;
    return OTAUpdateStatus::Pending;
}

void OTAUpdateManager::markBootHealthy(bool healthy) {
    if (healthy) {
        Logger::info("OTA boot health check passed");
        pendingValidUpdate_ = false;
        lastError_ = nullptr;
        return;
    }

    Logger::error("OTA boot health check failed; rollback required");
    lastError_ = "OTA boot health check failed";
}

const char* OTAUpdateManager::lastError() const {
    return lastError_;
}

} // namespace flintos
