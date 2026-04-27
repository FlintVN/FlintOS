#include "core/service_registry.hpp"

#include "core/logger.hpp"

namespace flintos {

void ServiceRegistry::initialize(PermissionManager& permissions,
                                 AppRegistry& apps,
                                 SettingsStore& settings,
                                 PackageManager& packages,
                                 OTAUpdateManager& ota) {
    Logger::info("Initializing FlintOS service registry");
    permissions_ = &permissions;
    apps_ = &apps;
    settings_ = &settings;
    packages_ = &packages;
    ota_ = &ota;
    initialized_ = true;
}

bool ServiceRegistry::isInitialized() const {
    return initialized_;
}

PermissionManager& ServiceRegistry::permissions() const {
    return *permissions_;
}

AppRegistry& ServiceRegistry::apps() const {
    return *apps_;
}

SettingsStore& ServiceRegistry::settings() const {
    return *settings_;
}

PackageManager& ServiceRegistry::packages() const {
    return *packages_;
}

OTAUpdateManager& ServiceRegistry::ota() const {
    return *ota_;
}

} // namespace flintos
