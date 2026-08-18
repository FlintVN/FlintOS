
#ifndef __FLINTOS_NATIVE_AUDIO_H
#define __FLINTOS_NATIVE_AUDIO_H

#include "flint_native.h"

jint NativeAudioTrack_GetFrameSize(FNIEnv *env);
jint NativeAudioTrack_GetVolumn(FNIEnv *env);
jvoid NativeAudioTrack_SetVolumn(FNIEnv *env, jint value);
jint NativeAudioTrack_Open0(FNIEnv *env, jobject obj);
jvoid NativeAudioTrack_Write1(FNIEnv *env, jobject obj, jbyteArray b);
jvoid NativeAudioTrack_Write2(FNIEnv *env, jobject obj, jbyteArray b, jint off, jint len);

inline constexpr NativeMethod audioTrackMethods[] = {
    NATIVE_METHOD("getFrameSize",  "()I",     NativeAudioTrack_GetFrameSize),
    NATIVE_METHOD("getVolumn",     "()I",     NativeAudioTrack_GetVolumn),
    NATIVE_METHOD("setVolumn",     "(I)V",    NativeAudioTrack_SetVolumn),
    NATIVE_METHOD("open0",         "()I",     NativeAudioTrack_Open0),
    NATIVE_METHOD("write",         "([B)V",   NativeAudioTrack_Write1),
    NATIVE_METHOD("write",         "([BII)V", NativeAudioTrack_Write2),
};

#endif /* __FLINTOS_NATIVE_AUDIO_H */
