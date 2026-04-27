#include "core/permission_manager.hpp"

#include <cstddef>
#include <cstring>

namespace flintos {

namespace {

bool equals(const char* left, const char* right) {
    return left != nullptr && right != nullptr && std::strcmp(left, right) == 0;
}

} // namespace

bool PermissionManager::hasPermission(const AppManifest& manifest, const char* permission) const {
    if (permission == nullptr) {
        return false;
    }

    for (std::size_t index = 0; index < manifest.permissionCount; ++index) {
        if (equals(manifest.permissions[index], permission)) {
            return true;
        }
    }

    return false;
}

bool PermissionManager::hasPermission(const char* appId, const char* permission) const {
    return isAllowed(appId, permission);
}

bool PermissionManager::canLaunch(const char* callerId, const AppManifest& target) const {
    return target.id != nullptr && target.mainClass != nullptr && isAllowed(callerId, "app.launch");
}

bool PermissionManager::isAllowed(const char* appId, const char* permission) const {
    if (permission == nullptr) {
        return false;
    }

    if (equals(appId, "flintos.launcher")) {
        return equals(permission, "app.launch") ||
               equals(permission, "system.info") ||
               equals(permission, "ui.screen") ||
               equals(permission, "settings.read");
    }

    if (equals(appId, "flintos.settings")) {
        return equals(permission, "system.info") ||
               equals(permission, "ui.screen") ||
               equals(permission, "settings.read") ||
               equals(permission, "settings.write");
    }

    if (equals(appId, "flintos.diagnostics")) {
        return equals(permission, "system.info") ||
               equals(permission, "ui.screen");
    }

    if (equals(appId, "flintos.updater")) {
        return equals(permission, "system.info") ||
               equals(permission, "ui.screen") ||
               equals(permission, "ota.update");
    }

    return false;
}

} // namespace flintos
