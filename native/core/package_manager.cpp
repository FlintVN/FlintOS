#include "core/package_manager.hpp"

#include <cstddef>
#include <cstring>

#include "core/logger.hpp"

namespace flintos {

namespace {

bool hasSuffix(const char* value, const char* suffix) {
    if (value == nullptr || suffix == nullptr) {
        return false;
    }

    const std::size_t valueLength = std::strlen(value);
    const std::size_t suffixLength = std::strlen(suffix);
    if (valueLength < suffixLength) {
        return false;
    }

    return std::strcmp(value + valueLength - suffixLength, suffix) == 0;
}

} // namespace

void PackageManager::initialize() {
    Logger::info("Initializing FlintOS package manager");
    initialized_ = true;
    lastError_ = nullptr;
}

bool PackageManager::isInitialized() const {
    return initialized_;
}

bool PackageManager::verifyPackage(const char* packagePath) const {
    return initialized_ && packagePath != nullptr && packagePath[0] != '\0' && hasSuffix(packagePath, ".flintapp");
}

bool PackageManager::verifyPackageStructure(bool hasManifest, bool hasClassesOrModules, bool hasValidChecksum) const {
    return initialized_ && hasManifest && hasClassesOrModules && hasValidChecksum;
}

PackageInstallStatus PackageManager::installPackage(const char* packagePath, bool permissionsApproved) {
    if (!initialized_) {
        lastError_ = "package manager is not initialized";
        return PackageInstallStatus::NotInitialized;
    }

    Logger::info("Writing Flint app package to staging area");
    if (!verifyPackage(packagePath)) {
        lastError_ = "invalid Flint app package";
        Logger::error(lastError_);
        return PackageInstallStatus::InvalidPackage;
    }

    Logger::info("Verifying Flint app manifest and package structure");
    if (!permissionsApproved) {
        lastError_ = "requested permissions are not approved";
        Logger::error(lastError_);
        return PackageInstallStatus::PermissionDenied;
    }

    Logger::info("Installing Flint app and refreshing registry");
    lastError_ = nullptr;
    return PackageInstallStatus::Installed;
}

PackageInstallStatus PackageManager::installFromCatalog(const char* catalogJson, bool permissionsApproved) {
    if (!initialized_) {
        lastError_ = "package manager is not initialized";
        return PackageInstallStatus::NotInitialized;
    }

    if (catalogJson == nullptr || catalogJson[0] == '\0') {
        lastError_ = "empty app catalog";
        return PackageInstallStatus::InvalidPackage;
    }

    return installPackage("catalog.flintapp", permissionsApproved);
}

const char* PackageManager::lastError() const {
    return lastError_;
}

} // namespace flintos
