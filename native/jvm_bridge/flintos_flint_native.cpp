#if FLINTOS_HAS_FLINTESPJVM

#include <cstddef>
#include <cstring>

#include "core/app_registry.hpp"
#include "core/app_manager_native.hpp"
#include "core/download_manager.hpp"
#include "core/input_event_dispatcher.hpp"
#include "core/logger.hpp"
#include "core/ota_update_manager.hpp"
#include "core/permission_manager.hpp"
#include "core/settings_store.hpp"
#include "core/storage_service.hpp"
#include "core/task_manager.hpp"
#include "hal/board_profile.hpp"
#include "hal/display_driver.hpp"
#include "hal/input_driver.hpp"
#include "hal/network_driver.hpp"
#include "hal/power_manager.hpp"
#include "flint_array_object.h"
#include "flint_class_loader.h"
#include "flint_method_info.h"
#include "flint_native.h"
#include "flint_system_api.h"

namespace {

const char* copyString(jstring text, char* buffer, std::size_t bufferSize) {
    if (bufferSize == 0) {
        return "";
    }
    if (text == nullptr) {
        buffer[0] = '\0';
        return buffer;
    }

    const uint32_t length = text->getLength();
    const uint32_t copyLength = length < bufferSize ? length : bufferSize - 1;
    std::memcpy(buffer, text->getAscii(), copyLength);
    buffer[copyLength] = '\0';
    return buffer;
}

jobjectArray createAppInfoArray(FNIEnv* env, const flintos::AppManifest* const* apps, std::size_t count) {
    jclass appInfoClass = env->findClass("flint/app/AppInfo");
    jmethodId ctor = env->getConstructorId(appInfoClass, "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
    jobjectArray result = env->newObjectArray(appInfoClass, static_cast<jint>(count));
    if (result == nullptr || ctor == nullptr || apps == nullptr) {
        return result;
    }

    JObject** data = result->getData();
    for (std::size_t index = 0; index < count; index++) {
        const flintos::AppManifest* app = apps[index];
        if (app == nullptr) {
            continue;
        }
        data[index] = env->newObject(
            appInfoClass,
            ctor,
            env->newString(app->id),
            env->newString(app->name),
            env->newString(app->mainClass));
    }
    return result;
}

jobjectArray createAppInfoArray(FNIEnv* env, const flintos::AppManifest* apps, std::size_t count) {
    const flintos::AppManifest* refs[16] = {};
    const std::size_t copyCount = count < 16 ? count : 16;
    for (std::size_t index = 0; index < copyCount; index++) {
        refs[index] = &apps[index];
    }
    return createAppInfoArray(env, refs, copyCount);
}

jstring nativeSystemInfoOsVersion(FNIEnv* env) {
    return env->newString("0.1.0");
}

jstring nativeSystemInfoBoardName(FNIEnv* env) {
    flintos::BoardProfile board;
    return env->newString(board.name());
}

void nativeScreenClear(FNIEnv*) {
    flintos::DisplayDriver display;
    display.clear();
}

void nativeScreenDrawText(FNIEnv*, jstring text, jint x, jint y) {
    char buffer[128];
    flintos::DisplayDriver display;
    display.drawText(copyString(text, buffer, sizeof(buffer)), x, y);
}

jobjectArray nativeAppManagerInstalledApps(FNIEnv* env) {
    flintos::AppRegistry registry;
    registry.initialize();
    std::size_t count = 0;
    const flintos::AppManifest* apps = registry.installedApps(&count);
    return createAppInfoArray(env, apps, count);
}

jbool nativeAppManagerLaunch(FNIEnv*, jstring appId) {
    char appIdBuffer[96];
    flintos::AppRegistry registry;
    flintos::PermissionManager permissions;
    flintos::AppManagerNative appManager;
    registry.initialize();
    appManager.initialize(registry, permissions);
    return appManager.startActivity("flintos.launcher", copyString(appId, appIdBuffer, sizeof(appIdBuffer)), nullptr);
}

void nativeAppManagerFinishActivity(FNIEnv*) {
    flintos::AppRegistry registry;
    flintos::PermissionManager permissions;
    flintos::AppManagerNative appManager;
    registry.initialize();
    appManager.initialize(registry, permissions);
    appManager.finishActivity();
}

void nativeAppManagerRegisterIntentHandler(FNIEnv*, jstring action, jstring appId) {
    char actionBuffer[96];
    char appIdBuffer[96];
    flintos::AppRegistry registry;
    flintos::PermissionManager permissions;
    flintos::AppManagerNative appManager;
    registry.initialize();
    appManager.initialize(registry, permissions);
    appManager.registerIntentHandler(
        copyString(action, actionBuffer, sizeof(actionBuffer)),
        copyString(appId, appIdBuffer, sizeof(appIdBuffer)));
}

jobjectArray nativeAppManagerQueryIntentActivities(FNIEnv* env, jobject intent) {
    flintos::AppRegistry registry;
    flintos::PermissionManager permissions;
    flintos::AppManagerNative appManager;
    registry.initialize();
    appManager.initialize(registry, permissions);

    jstring action = nullptr;
    if (intent != nullptr) {
        jclass intentClass = env->findClass("flint/app/Intent");
        jmethodId actionMethod = env->getMethodId(intentClass, "action", "()Ljava/lang/String;");
        action = static_cast<jstring>(env->callObjectMethod(actionMethod, intent));
    }

    char actionBuffer[96];
    std::size_t count = 0;
    const flintos::AppManifest* const* apps = appManager.queryIntentActivities(copyString(action, actionBuffer, sizeof(actionBuffer)), &count);
    return createAppInfoArray(env, apps, count);
}

void nativeLoggerInfo(FNIEnv*, jstring message) {
    char buffer[160];
    flintos::Logger::info(copyString(message, buffer, sizeof(buffer)));
}

void nativeLoggerWarn(FNIEnv*, jstring message) {
    char buffer[160];
    flintos::Logger::warn(copyString(message, buffer, sizeof(buffer)));
}

void nativeLoggerError(FNIEnv*, jstring message) {
    char buffer[160];
    flintos::Logger::error(copyString(message, buffer, sizeof(buffer)));
}

jstring nativeSettingsGetString(FNIEnv* env, jstring key, jstring fallback) {
    char keyBuffer[96];
    char fallbackBuffer[128];
    flintos::SettingsStore settings;
    settings.initialize();
    return env->newString(settings.getString(
        copyString(key, keyBuffer, sizeof(keyBuffer)),
        copyString(fallback, fallbackBuffer, sizeof(fallbackBuffer))));
}

jbool nativeUpdateManagerHasPendingValidUpdate(FNIEnv*) {
    flintos::OTAUpdateManager ota;
    ota.initialize();
    return ota.hasPendingValidUpdate();
}

jbool nativeUpdateManagerStageUpdate(FNIEnv*, jstring metadataUrl, jstring expectedChecksum) {
    char metadataBuffer[192];
    char checksumBuffer[96];
    flintos::OTAUpdateManager ota;
    ota.initialize();
    return ota.stageUpdate(
        copyString(metadataUrl, metadataBuffer, sizeof(metadataBuffer)),
        copyString(expectedChecksum, checksumBuffer, sizeof(checksumBuffer))) == flintos::OTAUpdateStatus::Pending;
}

void nativeUpdateManagerMarkBootHealthy(FNIEnv*, jbool healthy) {
    flintos::OTAUpdateManager ota;
    ota.initialize();
    ota.markBootHealthy(healthy);
}

jbool nativeStorageExists(FNIEnv*, jstring path) {
    char pathBuffer[128];
    flintos::StorageService storage;
    storage.initialize();
    return storage.exists(copyString(path, pathBuffer, sizeof(pathBuffer)));
}

jstring nativeStorageRootPath(FNIEnv* env) {
    flintos::StorageService storage;
    storage.initialize();
    return env->newString(storage.rootPath());
}

jbool nativeWiFiIsEnabled(FNIEnv*) {
    flintos::NetworkDriver network;
    network.initialize();
    return network.isWifiEnabled();
}

jbool nativeWiFiConnect(FNIEnv*, jstring ssid) {
    char ssidBuffer[96];
    flintos::NetworkDriver network;
    network.initialize();
    return network.connectWifi(copyString(ssid, ssidBuffer, sizeof(ssidBuffer)));
}

jbool nativeInputHasPendingEvent(FNIEnv*) {
    flintos::InputDriver input;
    input.initialize();
    return input.hasPendingEvent();
}

jbool nativeDownloadManagerDownloadUrl(FNIEnv*, jstring url, jstring outputPath) {
    char urlBuffer[192];
    char outputPathBuffer[128];
    flintos::DownloadManager downloads;
    downloads.initialize();
    return downloads.downloadUrl(
        copyString(url, urlBuffer, sizeof(urlBuffer)),
        copyString(outputPath, outputPathBuffer, sizeof(outputPathBuffer)));
}

jstring nativeDownloadManagerDownloadText(FNIEnv* env, jstring url, jint timeoutMs) {
    char urlBuffer[192];
    flintos::DownloadManager downloads;
    downloads.initialize();
    return env->newString(downloads.downloadText(
        copyString(url, urlBuffer, sizeof(urlBuffer)),
        static_cast<uint32_t>(timeoutMs)));
}

jint nativePowerBatteryPercent(FNIEnv*) {
    flintos::PowerManager power;
    power.initialize();
    return power.isInitialized() ? 100 : 0;
}

jbool nativePowerIsCharging(FNIEnv*) {
    flintos::PowerManager power;
    power.initialize();
    return power.isInitialized();
}

jint nativeTaskManagerCreateTask(FNIEnv*, jstring taskName) {
    char taskNameBuffer[96];
    flintos::TaskManager tasks;
    tasks.initialize();
    return tasks.createTask(copyString(taskName, taskNameBuffer, sizeof(taskNameBuffer)));
}

void nativeTaskManagerCancelTask(FNIEnv*, jint taskId) {
    flintos::TaskManager tasks;
    tasks.initialize();
    tasks.cancelTask(taskId);
}

jbool nativeTaskManagerIsTaskActive(FNIEnv*, jint taskId) {
    flintos::TaskManager tasks;
    tasks.initialize();
    return tasks.isTaskActive(taskId);
}

void nativeTaskManagerSleepMs(FNIEnv*, jint milliseconds) {
    flintos::TaskManager tasks;
    tasks.initialize();
    tasks.sleepMs(static_cast<uint32_t>(milliseconds));
}

void nativePinWrite(FNIEnv*, jint, jbool) {
    flintos::Logger::info("GPIO write requested");
}

jbool nativePinRead(FNIEnv*, jint) {
    return false;
}

jbool nativeBusIsAvailable(FNIEnv*, jint) {
    return true;
}

constexpr NativeMethod SYSTEM_INFO_METHODS[] = {
    NATIVE_METHOD("osVersion", "()Ljava/lang/String;", nativeSystemInfoOsVersion),
    NATIVE_METHOD("boardName", "()Ljava/lang/String;", nativeSystemInfoBoardName),
};

constexpr NativeMethod SCREEN_METHODS[] = {
    NATIVE_METHOD("clear", "()V", nativeScreenClear),
    NATIVE_METHOD("drawText", "(Ljava/lang/String;II)V", nativeScreenDrawText),
};

constexpr NativeMethod APP_MANAGER_METHODS[] = {
    NATIVE_METHOD("installedApps", "()[Lflint/app/AppInfo;", nativeAppManagerInstalledApps),
    NATIVE_METHOD("launch", "(Ljava/lang/String;)Z", nativeAppManagerLaunch),
    NATIVE_METHOD("finishActivity", "()V", nativeAppManagerFinishActivity),
    NATIVE_METHOD("registerIntentHandler", "(Ljava/lang/String;Ljava/lang/String;)V", nativeAppManagerRegisterIntentHandler),
    NATIVE_METHOD("queryIntentActivities", "(Lflint/app/Intent;)[Lflint/app/AppInfo;", nativeAppManagerQueryIntentActivities),
};

constexpr NativeMethod LOGGER_METHODS[] = {
    NATIVE_METHOD("info", "(Ljava/lang/String;)V", nativeLoggerInfo),
    NATIVE_METHOD("warn", "(Ljava/lang/String;)V", nativeLoggerWarn),
    NATIVE_METHOD("error", "(Ljava/lang/String;)V", nativeLoggerError),
};

constexpr NativeMethod SETTINGS_METHODS[] = {
    NATIVE_METHOD("getString", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;", nativeSettingsGetString),
};

constexpr NativeMethod UPDATE_MANAGER_METHODS[] = {
    NATIVE_METHOD("hasPendingValidUpdate", "()Z", nativeUpdateManagerHasPendingValidUpdate),
    NATIVE_METHOD("stageUpdate", "(Ljava/lang/String;Ljava/lang/String;)Z", nativeUpdateManagerStageUpdate),
    NATIVE_METHOD("markBootHealthy", "(Z)V", nativeUpdateManagerMarkBootHealthy),
};

constexpr NativeMethod STORAGE_METHODS[] = {
    NATIVE_METHOD("exists", "(Ljava/lang/String;)Z", nativeStorageExists),
    NATIVE_METHOD("rootPath", "()Ljava/lang/String;", nativeStorageRootPath),
};

constexpr NativeMethod WIFI_METHODS[] = {
    NATIVE_METHOD("isEnabled", "()Z", nativeWiFiIsEnabled),
    NATIVE_METHOD("connect", "(Ljava/lang/String;)Z", nativeWiFiConnect),
};

constexpr NativeMethod INPUT_METHODS[] = {
    NATIVE_METHOD("hasPendingEvent", "()Z", nativeInputHasPendingEvent),
};

constexpr NativeMethod DOWNLOAD_MANAGER_METHODS[] = {
    NATIVE_METHOD("downloadUrl", "(Ljava/lang/String;Ljava/lang/String;)Z", nativeDownloadManagerDownloadUrl),
    NATIVE_METHOD("downloadText", "(Ljava/lang/String;I)Ljava/lang/String;", nativeDownloadManagerDownloadText),
};

constexpr NativeMethod POWER_METHODS[] = {
    NATIVE_METHOD("batteryPercent", "()I", nativePowerBatteryPercent),
    NATIVE_METHOD("isCharging", "()Z", nativePowerIsCharging),
};

constexpr NativeMethod TASK_MANAGER_METHODS[] = {
    NATIVE_METHOD("createTask", "(Ljava/lang/String;)I", nativeTaskManagerCreateTask),
    NATIVE_METHOD("cancelTask", "(I)V", nativeTaskManagerCancelTask),
    NATIVE_METHOD("isTaskActive", "(I)Z", nativeTaskManagerIsTaskActive),
    NATIVE_METHOD("sleepMs", "(I)V", nativeTaskManagerSleepMs),
};

constexpr NativeMethod PIN_METHODS[] = {
    NATIVE_METHOD("write", "(IZ)V", nativePinWrite),
    NATIVE_METHOD("read", "(I)Z", nativePinRead),
};

constexpr NativeMethod BUS_METHODS[] = {
    NATIVE_METHOD("isAvailable", "(I)Z", nativeBusIsAvailable),
};

constexpr NativeClass FLINTOS_NATIVE_CLASSES[] = {
    NATIVE_CLASS("flint/os/SystemInfo", SYSTEM_INFO_METHODS),
    NATIVE_CLASS("flint/ui/Screen", SCREEN_METHODS),
    NATIVE_CLASS("flint/app/AppManager", APP_MANAGER_METHODS),
    NATIVE_CLASS("flint/os/Logger", LOGGER_METHODS),
    NATIVE_CLASS("flint/os/Settings", SETTINGS_METHODS),
    NATIVE_CLASS("flint/os/UpdateManager", UPDATE_MANAGER_METHODS),
    NATIVE_CLASS("flint/os/Storage", STORAGE_METHODS),
    NATIVE_CLASS("flint/net/WiFi", WIFI_METHODS),
    NATIVE_CLASS("flint/ui/Input", INPUT_METHODS),
    NATIVE_CLASS("flint/os/DownloadManager", DOWNLOAD_MANAGER_METHODS),
    NATIVE_CLASS("flint/os/Power", POWER_METHODS),
    NATIVE_CLASS("flint/os/TaskManager", TASK_MANAGER_METHODS),
    NATIVE_CLASS("flint/io/Pin", PIN_METHODS),
    NATIVE_CLASS("flint/io/I2C", BUS_METHODS),
    NATIVE_CLASS("flint/io/SPI", BUS_METHODS),
};

} // namespace

void FlintAPI::System::reset(void) {
}

JNMPtr FlintAPI::System::findNativeMethod(MethodInfo* methodInfo) {
    const uint32_t classNameHash = methodInfo->loader->getHashKey();
    for (const NativeClass& nativeClass : FLINTOS_NATIVE_CLASSES) {
        if (classNameHash != nativeClass.hash || std::strcmp(nativeClass.className, methodInfo->loader->getName()) != 0) {
            continue;
        }

        for (uint32_t index = 0; index < nativeClass.methodCount; ++index) {
            const NativeMethod& method = nativeClass.methods[index];
            if (method.hash == methodInfo->hash &&
                std::strcmp(method.name, methodInfo->name) == 0 &&
                std::strcmp(method.desc, methodInfo->desc) == 0) {
                return reinterpret_cast<JNMPtr>(method.methodPtr);
            }
        }
    }

    return nullptr;
}

#endif
