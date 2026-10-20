
#ifndef __FLINTOS_NATIVE_AUDIO_MANAGER_H
#define __FLINTOS_NATIVE_AUDIO_MANAGER_H

#include "flint_native.h"

jint NativeAudioManager_GetVolume(FNIEnv *env);
jvoid NativeAudioManager_SetVolume(FNIEnv *env, jint value);

inline constexpr NativeMethod audioManagerMethods[] = {
    NATIVE_METHOD("getVolume", "()I",  NativeAudioManager_GetVolume),
    NATIVE_METHOD("setVolume", "(I)V", NativeAudioManager_SetVolume),
};

#endif /* __FLINTOS_NATIVE_AUDIO_MANAGER_H */
