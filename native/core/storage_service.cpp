#include "core/storage_service.hpp"

#include "core/logger.hpp"

namespace flintos {

void StorageService::initialize() {
    Logger::info("Initializing storage service");
    initialized_ = true;
}

bool StorageService::isInitialized() const {
    return initialized_;
}

bool StorageService::exists(const char* path) const {
    return initialized_ && path != nullptr && path[0] != '\0';
}

const char* StorageService::rootPath() const {
    return "/";
}

} // namespace flintos
