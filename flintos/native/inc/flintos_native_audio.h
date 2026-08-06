
#ifndef __FLINTOS_NATIVE_AUDIO_H
#define __FLINTOS_NATIVE_AUDIO_H

#include "flint_native.h"

jint NativeAudio_GetSampleRate(FNIEnv *env);
jint NativeAudio_GetChannels(FNIEnv *env);
jint NativeAudio_GetFrameSize(FNIEnv *env);
jint NativeAudio_GetVolumn(FNIEnv *env);
jvoid NativeAudio_SetVolumn(FNIEnv *env, jint value);
jvoid NativeAudio_Open(FNIEnv *env, jobject obj);
jvoid NativeAudio_Write1(FNIEnv *env, jobject obj, jbyteArray b);
jvoid NativeAudio_Write2(FNIEnv *env, jobject obj, jbyteArray b, jint off, jint len);
jvoid NativeAudio_Close(FNIEnv *env, jobject obj);

inline constexpr NativeMethod audioMethods[] = {
    NATIVE_METHOD("getSampleRate", "()I",     NativeAudio_GetSampleRate),
    NATIVE_METHOD("getChannels",   "()I",     NativeAudio_GetChannels),
    NATIVE_METHOD("getFrameSize",  "()I",     NativeAudio_GetFrameSize),
    NATIVE_METHOD("getVolumn",     "()I",     NativeAudio_GetVolumn),
    NATIVE_METHOD("setVolumn",     "(I)V",    NativeAudio_SetVolumn),
    NATIVE_METHOD("open",          "()V",     NativeAudio_Open),
    NATIVE_METHOD("write",         "([B)V",   NativeAudio_Write1),
    NATIVE_METHOD("write",         "([BII)V", NativeAudio_Write2),
    NATIVE_METHOD("close",         "()V",     NativeAudio_Close),
};

#endif /* __FLINTOS_NATIVE_AUDIO_H */
