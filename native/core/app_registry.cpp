#include "core/app_registry.hpp"

#include <cstring>

#include "core/logger.hpp"

namespace flintos {

namespace {

constexpr const char* LAUNCHER_PERMISSIONS[] = {
    "app.launch",
    "system.info",
    "ui.screen",
    "settings.read",
};

constexpr const char* SETTINGS_PERMISSIONS[] = {
    "system.info",
    "ui.screen",
    "settings.read",
    "settings.write",
};

constexpr const char* DIAGNOSTICS_PERMISSIONS[] = {
    "system.info",
    "ui.screen",
};

constexpr const char* UPDATER_PERMISSIONS[] = {
    "system.info",
    "ui.screen",
    "ota.update",
};

constexpr AppManifest SYSTEM_APPS[] = {
    {
        "flintos.launcher",
        "Launcher",
        "0.1.0",
        "system",
        "flintjvm",
        "flintos.launcher.LauncherApp",
        LAUNCHER_PERMISSIONS,
        sizeof(LAUNCHER_PERMISSIONS) / sizeof(LAUNCHER_PERMISSIONS[0]),
        "0.1.0",
        "0.1.0",
        "builtin:flintos.launcher",
        AppTrustState::Trusted,
    },
    {
        "flintos.settings",
        "Settings",
        "0.1.0",
        "system",
        "flintjvm",
        "flintos.settings.SettingsApp",
        SETTINGS_PERMISSIONS,
        sizeof(SETTINGS_PERMISSIONS) / sizeof(SETTINGS_PERMISSIONS[0]),
        "0.1.0",
        "0.1.0",
        "builtin:flintos.settings",
        AppTrustState::Trusted,
    },
    {
        "flintos.diagnostics",
        "Diagnostics",
        "0.1.0",
        "system",
        "flintjvm",
        "flintos.diagnostics.DiagnosticsApp",
        DIAGNOSTICS_PERMISSIONS,
        sizeof(DIAGNOSTICS_PERMISSIONS) / sizeof(DIAGNOSTICS_PERMISSIONS[0]),
        "0.1.0",
        "0.1.0",
        "builtin:flintos.diagnostics",
        AppTrustState::Trusted,
    },
    {
        "flintos.updater",
        "Updater",
        "0.1.0",
        "system",
        "flintjvm",
        "flintos.updater.UpdaterApp",
        UPDATER_PERMISSIONS,
        sizeof(UPDATER_PERMISSIONS) / sizeof(UPDATER_PERMISSIONS[0]),
        "0.1.0",
        "0.1.0",
        "builtin:flintos.updater",
        AppTrustState::Trusted,
    },
};

bool equals(const char* left, const char* right) {
    return left != nullptr && right != nullptr && std::strcmp(left, right) == 0;
}

bool isPresent(const char* value) {
    return value != nullptr && value[0] != '\0';
}

bool isValidType(const char* type) {
    return equals(type, "system") || equals(type, "user");
}

bool hasReadablePermissions(const AppManifest& manifest) {
    return manifest.permissions != nullptr || manifest.permissionCount == 0;
}

} // namespace

void AppRegistry::initialize() {
    Logger::info("Initializing FlintOS app registry");
    refresh();
    initialized_ = true;
}

void AppRegistry::refresh() {
    Logger::info("Refreshing FlintOS app registry");
}

bool AppRegistry::isInitialized() const {
    return initialized_;
}

bool AppRegistry::validate(const AppManifest& manifest) const {
    return isPresent(manifest.id) &&
           isPresent(manifest.name) &&
           isPresent(manifest.version) &&
           isValidType(manifest.type) &&
           equals(manifest.runtime, "flintjvm") &&
           isPresent(manifest.mainClass) &&
           hasReadablePermissions(manifest) &&
           isPresent(manifest.minOs) &&
           isPresent(manifest.minJdk) &&
           isPresent(manifest.checksum);
}

const AppManifest* AppRegistry::installedApps(std::size_t* count) const {
    return systemApps(count);
}

const AppManifest* AppRegistry::systemApps(std::size_t* count) const {
    if (count != nullptr) {
        *count = sizeof(SYSTEM_APPS) / sizeof(SYSTEM_APPS[0]);
    }

    return SYSTEM_APPS;
}

const AppManifest* AppRegistry::findById(const char* appId) const {
    for (const AppManifest& app : SYSTEM_APPS) {
        if (equals(app.id, appId)) {
            return &app;
        }
    }

    return nullptr;
}

} // namespace flintos
