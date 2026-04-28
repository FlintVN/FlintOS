#pragma once

namespace flintos {

enum class PackageInstallStatus {
    Installed,
    NotInitialized,
    InvalidPackage,
    PermissionDenied,
};

class PackageManager {
public:
    void initialize();
    bool isInitialized() const;
    bool verifyPackage(const char* packagePath) const;
    bool verifyPackageStructure(bool hasManifest, bool hasClassesOrModules, bool hasValidChecksum) const;
    PackageInstallStatus installPackage(const char* packagePath, bool permissionsApproved);
    PackageInstallStatus installFromCatalog(const char* catalogJson, bool permissionsApproved);
    const char* lastError() const;

private:
    bool initialized_ = false;
    const char* lastError_ = nullptr;
};

} // namespace flintos
