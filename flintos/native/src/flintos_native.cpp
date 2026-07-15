
#include <string.h>
#include "flint_method_info.h"
#include "flint_system_api.h"
#include "flint_class_loader.h"
#include "flint_native.h"
#include "flintos_native_wifi.h"
#include "flintos_native_audio.h"
#include "flintos_native_display.h"
#include "flintos_native_midp.h"

static constexpr NativeClass ESP_NATIVE_CLASS_LIST[] = {
    NATIVE_CLASS("flint/net/WiFi",         wifiMethods),
    NATIVE_CLASS("flintos/device/Audio",   audioMethods),
    NATIVE_CLASS("flintos/device/Display", displayMethods),
    NATIVE_CLASS("board/Lcd",              midpLcdMethods),
    NATIVE_CLASS("board/Touch",            midpTouchMethods),
    NATIVE_CLASS("board/Png",              midpPngMethods),
};

void FlintAPI::System::reset(void) {

}

JNMPtr FlintAPI::System::findNativeMethod(MethodInfo *methodInfo) {
    uint32_t classNameHash = methodInfo->loader->getHashKey();
    for(uint32_t i = 0; i < LENGTH(ESP_NATIVE_CLASS_LIST); i++) {
        const NativeClass *nativeCls = &ESP_NATIVE_CLASS_LIST[i];
        if(
            classNameHash == nativeCls->hash &&
            strcmp(nativeCls->className, methodInfo->loader->getName()) == 0
        ) {
            for(uint32_t k = 0; k < nativeCls->methodCount; k++) {
                if(
                    nativeCls->methods[k].hash == methodInfo->hash &&
                    strcmp(nativeCls->methods[k].name, methodInfo->name) == 0 &&
                    strcmp(nativeCls->methods[k].desc, methodInfo->desc) == 0
                ) {
                    return (JNMPtr)nativeCls->methods[k].methodPtr;
                }
            }
            break;
        }
    }
    return NULL;
}
