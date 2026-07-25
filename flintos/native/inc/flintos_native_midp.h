
#ifndef __FLINTOS_NATIVE_MIDP_H
#define __FLINTOS_NATIVE_MIDP_H

#include "flint_native.h"

/* ---- flintos.midp.DisplayBridge ---- */
jvoid NativeMidpLcd_Init(FNIEnv *env);
jvoid NativeMidpLcd_InitEx(FNIEnv *env, jint width, jint height, jstring mode);
jint  NativeMidpLcd_Width(FNIEnv *env);
jint  NativeMidpLcd_Height(FNIEnv *env);
jvoid NativeMidpLcd_Present(FNIEnv *env, jbyteArray frameBuffer);
jint  NativeMidpLcd_ReadKey(FNIEnv *env);

/* ---- flintos.midp.TouchBridge ---- */
jvoid NativeMidpTouch_Init(FNIEnv *env);
jint  NativeMidpTouch_Poll(FNIEnv *env);
jvoid NativeMidpTouch_Boost(FNIEnv *env);

/* ---- flintos.midp.PngDecoder ---- */
jobject NativeMidpPng_Decode(FNIEnv *env, jbyteArray data, jint off, jint len, jintArray wh);

/* ---- registration tables ---- */

inline constexpr NativeMethod midpDisplayMethods[] = {
    NATIVE_METHOD("init",    "()V",                NativeMidpLcd_Init),
    NATIVE_METHOD("init",    "(IILjava/lang/String;)V", NativeMidpLcd_InitEx),
    NATIVE_METHOD("width",   "()I",                NativeMidpLcd_Width),
    NATIVE_METHOD("height",  "()I",                NativeMidpLcd_Height),
    NATIVE_METHOD("present", "([B)V",              NativeMidpLcd_Present),
    NATIVE_METHOD("readKey", "()I",                NativeMidpLcd_ReadKey),
};

inline constexpr NativeMethod midpTouchMethods[] = {
    NATIVE_METHOD("init",  "()V", NativeMidpTouch_Init),
    NATIVE_METHOD("poll",  "()I", NativeMidpTouch_Poll),
    NATIVE_METHOD("boost", "()V", NativeMidpTouch_Boost),
};

inline constexpr NativeMethod midpPngMethods[] = {
    NATIVE_METHOD("decode", "([BII[I)[B", NativeMidpPng_Decode),
};

#endif /* __FLINTOS_NATIVE_MIDP_H */
