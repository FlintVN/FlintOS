#pragma once

#include <cstddef>

#include "core/app_manifest.hpp"
#include "core/app_registry.hpp"
#include "core/permission_manager.hpp"

namespace flintos {

class AppManagerNative {
public:
    void initialize(AppRegistry& registry, PermissionManager& permissions);
    bool isInitialized() const;
    bool startActivity(const char* callerId, const char* targetAppId, const char* action);
    void finishActivity();
    bool registerIntentHandler(const char* action, const char* appId);
    const AppManifest* const* queryIntentActivities(const char* action, std::size_t* count) const;
    const AppManifest* foregroundApp() const;
    const char* lastError() const;

private:
    static constexpr std::size_t MaxStackDepth = 8;
    static constexpr std::size_t MaxIntentHandlers = 16;

    struct IntentHandler {
        const char* action = nullptr;
        const char* appId = nullptr;
    };

    bool initialized_ = false;
    AppRegistry* registry_ = nullptr;
    PermissionManager* permissions_ = nullptr;
    const AppManifest* activityStack_[MaxStackDepth] = {};
    std::size_t stackDepth_ = 0;
    IntentHandler handlers_[MaxIntentHandlers] = {};
    std::size_t handlerCount_ = 0;
    mutable const AppManifest* queryResults_[MaxIntentHandlers] = {};
    const char* lastError_ = nullptr;
};

} // namespace flintos
