#pragma once

#include "core/app_manifest.hpp"

namespace flintos {

class PermissionManager {
public:
    bool hasPermission(const AppManifest& manifest, const char* permission) const;
    bool hasPermission(const char* appId, const char* permission) const;
    bool canLaunch(const char* callerId, const AppManifest& target) const;
    bool isAllowed(const char* appId, const char* permission) const;
};

} // namespace flintos
