#include "core/app_manager_native.hpp"

#include <cstring>

#include "core/logger.hpp"

namespace flintos {

namespace {

bool equals(const char* left, const char* right) {
    return left != nullptr && right != nullptr && std::strcmp(left, right) == 0;
}

bool isPresent(const char* value) {
    return value != nullptr && value[0] != '\0';
}

} // namespace

void AppManagerNative::initialize(AppRegistry& registry, PermissionManager& permissions) {
    registry_ = &registry;
    permissions_ = &permissions;
    stackDepth_ = 0;
    handlerCount_ = 0;
    lastError_ = nullptr;
    initialized_ = true;

    registerIntentHandler("flintos.intent.action.MAIN", "flintos.launcher");
    registerIntentHandler("flintos.intent.action.SETTINGS", "flintos.settings");
    registerIntentHandler("flintos.intent.action.APP_STORE", "flintos.appstore");
    registerIntentHandler("flintos.intent.action.UPDATE", "flintos.updater");
}

bool AppManagerNative::isInitialized() const {
    return initialized_;
}

bool AppManagerNative::startActivity(const char* callerId, const char* targetAppId, const char* action) {
    if (!initialized_) {
        lastError_ = "app manager is not initialized";
        return false;
    }

    const AppManifest* target = isPresent(targetAppId) ? registry_->findById(targetAppId) : nullptr;
    if (target == nullptr && isPresent(action)) {
        std::size_t count = 0;
        const AppManifest* const* handlers = queryIntentActivities(action, &count);
        target = count == 1 ? handlers[0] : nullptr;
    }

    if (target == nullptr) {
        lastError_ = "activity target not found";
        return false;
    }

    if (stackDepth_ >= MaxStackDepth) {
        lastError_ = "activity stack is full";
        return false;
    }

    const char* resolvedCaller = isPresent(callerId) ? callerId : "flintos.launcher";
    if (!permissions_->canLaunch(resolvedCaller, *target)) {
        lastError_ = "activity launch denied";
        return false;
    }

    activityStack_[stackDepth_++] = target;
    Logger::info("Starting FlintOS activity");
    Logger::info(target->mainClass);
    lastError_ = nullptr;
    return true;
}

void AppManagerNative::finishActivity() {
    if (stackDepth_ > 0) {
        --stackDepth_;
    }
}

bool AppManagerNative::registerIntentHandler(const char* action, const char* appId) {
    if (!isPresent(action) || !isPresent(appId)) {
        lastError_ = "invalid intent handler";
        return false;
    }

    if (registry_ != nullptr && registry_->findById(appId) == nullptr) {
        lastError_ = "intent handler app not found";
        return false;
    }

    for (std::size_t index = 0; index < handlerCount_; ++index) {
        if (equals(handlers_[index].action, action) && equals(handlers_[index].appId, appId)) {
            lastError_ = nullptr;
            return true;
        }
    }

    if (handlerCount_ >= MaxIntentHandlers) {
        lastError_ = "intent handler table is full";
        return false;
    }

    handlers_[handlerCount_++] = {action, appId};
    lastError_ = nullptr;
    return true;
}

const AppManifest* const* AppManagerNative::queryIntentActivities(const char* action, std::size_t* count) const {
    std::size_t resultCount = 0;
    if (isPresent(action) && registry_ != nullptr) {
        for (std::size_t index = 0; index < handlerCount_ && resultCount < MaxIntentHandlers; ++index) {
            if (equals(handlers_[index].action, action)) {
                const AppManifest* manifest = registry_->findById(handlers_[index].appId);
                if (manifest != nullptr) {
                    queryResults_[resultCount++] = manifest;
                }
            }
        }
    }

    if (count != nullptr) {
        *count = resultCount;
    }
    return queryResults_;
}

const AppManifest* AppManagerNative::foregroundApp() const {
    return stackDepth_ == 0 ? nullptr : activityStack_[stackDepth_ - 1];
}

const char* AppManagerNative::lastError() const {
    return lastError_;
}

} // namespace flintos
