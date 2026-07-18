
#ifndef __FLINTOS_NATIVE_MIDP_H
#define __FLINTOS_NATIVE_MIDP_H

#include "flint_native.h"

jvoid NativeMidpLcd_Init(FNIEnv *env);
jint NativeMidpLcd_Width(FNIEnv *env);
jint NativeMidpLcd_Height(FNIEnv *env);
jvoid NativeMidpLcd_Present(FNIEnv *env, jbyteArray frameBuffer);
jint NativeMidpLcd_ReadKey(FNIEnv *env);

jvoid NativeMidpTouch_Init(FNIEnv *env);
jint NativeMidpTouch_Poll(FNIEnv *env);
jvoid NativeMidpTouch_Boost(FNIEnv *env);

jbyteArray NativeMidpResourceLoader_ReadProgramResource(FNIEnv *env, jstring name);

jbyteArray NativeMidpPng_Decode(
    FNIEnv *env,
    jbyteArray data,
    jint offset,
    jint length,
    jintArray dimensions
);

inline constexpr NativeMethod midpLcdMethods[] = {
    NATIVE_METHOD("init",    "()V",   NativeMidpLcd_Init),
    NATIVE_METHOD("width",   "()I",   NativeMidpLcd_Width),
    NATIVE_METHOD("height",  "()I",   NativeMidpLcd_Height),
    NATIVE_METHOD("present", "([B)V", NativeMidpLcd_Present),
    NATIVE_METHOD("readKey", "()I",   NativeMidpLcd_ReadKey),
};

inline constexpr NativeMethod midpTouchMethods[] = {
    NATIVE_METHOD("init",  "()V", NativeMidpTouch_Init),
    NATIVE_METHOD("poll",  "()I", NativeMidpTouch_Poll),
    NATIVE_METHOD("boost", "()V", NativeMidpTouch_Boost),
};

inline constexpr NativeMethod midpPngMethods[] = {
    NATIVE_METHOD("decode", "([BII[I)[B", NativeMidpPng_Decode),
};

inline constexpr NativeMethod midpResourceLoaderMethods[] = {
    NATIVE_METHOD(
        "readProgramResource",
        "(Ljava/lang/String;)[B",
        NativeMidpResourceLoader_ReadProgramResource
    ),
};

inline constexpr NativeMethod systemResourceMethods[] = {
    NATIVE_METHOD(
        "getResourceBytes0",
        "(Ljava/lang/String;)[B",
        NativeMidpResourceLoader_ReadProgramResource
    ),
};

#endif /* __FLINTOS_NATIVE_MIDP_H */
