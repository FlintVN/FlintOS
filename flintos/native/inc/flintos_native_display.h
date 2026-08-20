
#ifndef __FLINTOS_NATIVE_DISPLAY_H
#define __FLINTOS_NATIVE_DISPLAY_H

#include "flint_native.h"

jint NativeDisplay_GetWidth(FNIEnv *env);
jint NativeDisplay_GetHeight(FNIEnv *env);
jbool NativeDisplay_IsForeground(FNIEnv *env);
jvoid NativeDisplay_Write(FNIEnv *env, jint x, jint y, jint w, jint h, jbyteArray data);
jvoid NativeDisplay_SetBrightness(FNIEnv *env, jint value);

inline constexpr NativeMethod displayMethods[] = {
    NATIVE_METHOD("getWidth",      "()I",       NativeDisplay_GetWidth),
    NATIVE_METHOD("getHeight",     "()I",       NativeDisplay_GetHeight),
    NATIVE_METHOD("isForeground",  "()Z",       NativeDisplay_IsForeground),
    NATIVE_METHOD("write",         "(IIII[B)V", NativeDisplay_Write),
    NATIVE_METHOD("setBrightness", "(I)V",      NativeDisplay_SetBrightness),
};

#endif /* __FLINTOS_NATIVE_DISPLAY_H */
