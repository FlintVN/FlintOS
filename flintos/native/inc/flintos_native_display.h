
#ifndef __FLINTOS_NATIVE_DISPLAY_H
#define __FLINTOS_NATIVE_DISPLAY_H

#include "flint_native.h"

jint NativeDisplay_GetPrimaryWidth(FNIEnv *env);
jint NativeDisplay_GetPrimaryHeight(FNIEnv *env);
jbool NativeDisplay_IsForeground(FNIEnv *env);
jvoid NativeDisplay_SetBrightness(FNIEnv *env, jint value);
jvoid NativeDisplay_Present1(FNIEnv *env, jobject obj);
jvoid NativeDisplay_Present2(FNIEnv *env, jobject obj, jint x, jint y, jint w, jint h);

inline constexpr NativeMethod displayMethods[] = {
    NATIVE_METHOD("getPrimaryWidth",  "()I",     NativeDisplay_GetPrimaryWidth),
    NATIVE_METHOD("getPrimaryHeight", "()I",     NativeDisplay_GetPrimaryHeight),
    NATIVE_METHOD("isForeground",     "()Z",     NativeDisplay_IsForeground),
    NATIVE_METHOD("setBrightness",    "(I)V",    NativeDisplay_SetBrightness),
    NATIVE_METHOD("present",          "()V",     NativeDisplay_Present1),
    NATIVE_METHOD("present",          "(IIII)V", NativeDisplay_Present2),
};

#endif /* __FLINTOS_NATIVE_DISPLAY_H */
