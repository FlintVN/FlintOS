#pragma once

#include "core/app_registry.hpp"
#include "core/ota_update_manager.hpp"
#include "core/package_manager.hpp"
#include "core/permission_manager.hpp"
#include "core/settings_store.hpp"

namespace flintos {

class ServiceRegistry {
public:
    void initialize(PermissionManager& permissions,
                    AppRegistry& apps,
                    SettingsStore& settings,
                    PackageManager& packages,
                    OTAUpdateManager& ota);
    bool isInitialized() const;

    PermissionManager& permissions() const;
    AppRegistry& apps() const;
    SettingsStore& settings() const;
    PackageManager& packages() const;
    OTAUpdateManager& ota() const;

private:
    bool initialized_ = false;
    PermissionManager* permissions_ = nullptr;
    AppRegistry* apps_ = nullptr;
    SettingsStore* settings_ = nullptr;
    PackageManager* packages_ = nullptr;
    OTAUpdateManager* ota_ = nullptr;
};

} // namespace flintos
