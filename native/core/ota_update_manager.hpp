#pragma once

namespace flintos {

enum class OTAUpdateStatus {
    Pending,
    NotInitialized,
    InvalidMetadata,
    VerificationFailed,
};

class OTAUpdateManager {
public:
    void initialize();
    bool isInitialized() const;
    bool hasPendingValidUpdate() const;
    OTAUpdateStatus stageUpdate(const char* metadataUrl, const char* expectedChecksum);
    void markBootHealthy(bool healthy);
    const char* lastError() const;

private:
    bool initialized_ = false;
    bool pendingValidUpdate_ = false;
    const char* lastError_ = nullptr;
};

} // namespace flintos
