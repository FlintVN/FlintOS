
#include "flintos_audio_service.h"
#include "flintos_native_audio_manager.h"

jint NativeAudioManager_GetVolume(FNIEnv *env) {
    (void)env;
    return AudioSrv::getVolume();
}

jvoid NativeAudioManager_SetVolume(FNIEnv *env, jint value) {
    (void)env;
    if (value < 0) value = 0;
    else if (value > 100) value = 100;
    AudioSrv::setVolume(value);
}
