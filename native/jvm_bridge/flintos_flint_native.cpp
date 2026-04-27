#if FLINTOS_HAS_FLINTESPJVM

#include <cstddef>
#include <cstring>

#include "core/app_registry.hpp"
#include "core/logger.hpp"
#include "core/ota_update_manager.hpp"
#include "core/settings_store.hpp"
#include "core/storage_service.hpp"
#include "hal/board_profile.hpp"
#include "hal/display_driver.hpp"
#include "hal/network_driver.hpp"
#include "flint_class_loader.h"
#include "flint_method_info.h"
#include "flint_native.h"
#include "flint_system_api.h"

namespace {

void nativeSystemInfoOsVersion(FNIEnv* env) {
    env->newString("0.1.0");
}

void nativeSystemInfoBoardName(FNIEnv* env) {
    flintos::BoardProfile board;
    env->newString(board.name());
}

void nativeScreenClear(FNIEnv*) {
    flintos::DisplayDriver display;
    display.clear();
}

void nativeScreenDrawText(FNIEnv*, jstring, jint x, jint y) {
    flintos::DisplayDriver display;
    display.drawText("<java-string>", x, y);
}

void nativeAppManagerInstalledApps(FNIEnv* env) {
    flintos::AppRegistry registry;
    registry.initialize();
    std::size_t count = 0;
    registry.installedApps(&count);

    jclass appInfoClass = env->findClass("flint/app/AppInfo");
    env->newObjectArray(appInfoClass, static_cast<jint>(count));
}

jbool nativeAppManagerLaunch(FNIEnv*, jstring) {
    flintos::Logger::info("Java AppManager launch request received");
    return true;
}

void nativeLoggerInfo(FNIEnv*, jstring) {
    flintos::Logger::info("<java-log>");
}

void nativeLoggerWarn(FNIEnv*, jstring) {
    flintos::Logger::warn("<java-log>");
}

void nativeLoggerError(FNIEnv*, jstring) {
    flintos::Logger::error("<java-log>");
}

void nativeSettingsGetString(FNIEnv* env, jstring, jstring) {
    flintos::SettingsStore settings;
    settings.initialize();
    env->newString(settings.getString("os.name", ""));
}

jbool nativeUpdateManagerHasPendingValidUpdate(FNIEnv*) {
    flintos::OTAUpdateManager ota;
    ota.initialize();
    return ota.hasPendingValidUpdate();
}

jbool nativeUpdateManagerStageUpdate(FNIEnv*, jstring, jstring) {
    flintos::OTAUpdateManager ota;
    ota.initialize();
    return ota.stageUpdate("metadata", "checksum") == flintos::OTAUpdateStatus::Pending;
}

void nativeUpdateManagerMarkBootHealthy(FNIEnv*, jbool healthy) {
    flintos::OTAUpdateManager ota;
    ota.initialize();
    ota.markBootHealthy(healthy);
}

jbool nativeStorageExists(FNIEnv*, jstring) {
    flintos::StorageService storage;
    storage.initialize();
    return storage.exists("/");
}

void nativeStorageRootPath(FNIEnv* env) {
    flintos::StorageService storage;
    storage.initialize();
    env->newString(storage.rootPath());
}

jbool nativeWiFiIsEnabled(FNIEnv*) {
    flintos::NetworkDriver network;
    network.initialize();
    return network.isWifiEnabled();
}

jbool nativeWiFiConnect(FNIEnv*, jstring) {
    flintos::NetworkDriver network;
    network.initialize();
    return network.connectWifi("ssid");
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
