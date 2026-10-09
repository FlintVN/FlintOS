

#ifndef __FLINTOS_NATIVE_PROCESS_H
#define __FLINTOS_NATIVE_PROCESS_H

#include "flint_native.h"

jvoid NativeProcess_Start(FNIEnv *env, jobject obj);
jvoid NativeProcess_Close(FNIEnv *env, jobject obj);
jobjectArray NativeProcess_GetProcesses(FNIEnv *env);

inline constexpr NativeMethod processMethods[] = {
    NATIVE_METHOD("start",        "()V",                       NativeProcess_Start),
    NATIVE_METHOD("close",        "()V",                       NativeProcess_Close),
    NATIVE_METHOD("getProcesses", "()[Lflint/system/Process;", NativeProcess_GetProcesses),
};

#endif /* __FLINTOS_NATIVE_PROCESS_H */
