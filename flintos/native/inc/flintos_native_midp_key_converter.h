
#ifndef __FLINTOS_NATIVE_MIDP_KEY_CONVERTER_H
#define __FLINTOS_NATIVE_MIDP_KEY_CONVERTER_H

#include "flint_native.h"

jint NativeKeyConverter_GetKeyCode(FNIEnv *env, jint gameAction);
jint NativeKeyConverter_GetGameAction(FNIEnv *env, jint keyCode);
jint NativeKeyConverter_GetSystemKey(FNIEnv *env, jint keyCode);
jstring NativeKeyConverter_GetKeyName(FNIEnv *env, jint keyCode);

inline constexpr NativeMethod midpKeyConverterMethods[] = {
    NATIVE_METHOD("getKeyCode",    "(I)I",                  NativeKeyConverter_GetKeyCode),
    NATIVE_METHOD("getGameAction", "(I)I",                  NativeKeyConverter_GetGameAction),
    NATIVE_METHOD("getSystemKey",  "(I)I",                  NativeKeyConverter_GetSystemKey),
    NATIVE_METHOD("getKeyName",    "(I)Ljava/lang/String;", NativeKeyConverter_GetKeyName),
};

#endif /* __FLINTOS_NATIVE_MIDP_KEY_CONVERTER_H */
