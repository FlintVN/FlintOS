
#ifndef __FLINTOS_NATIVE_NATIVE_EVENT_RECEIVER_H
#define __FLINTOS_NATIVE_NATIVE_EVENT_RECEIVER_H

#include "flint_native.h"

jbool NativeInputEvent_WaitEvent1(FNIEnv *env, jobject event);
jbool NativeInputEvent_WaitEvent2(FNIEnv *env, jobject event, jlong millis);
jvoid NativeInputEvent_NotifyEvent(FNIEnv *env);

inline constexpr NativeMethod nativeEventReceiverMethods[] = {
    NATIVE_METHOD("waitEvent",   "(Lflintos/system/NativeEvent;)Z",  NativeInputEvent_WaitEvent1),
    NATIVE_METHOD("waitEvent",   "(Lflintos/system/NativeEvent;J)Z", NativeInputEvent_WaitEvent2),
    NATIVE_METHOD("notifyEvent", "()V",                              NativeInputEvent_NotifyEvent),
};

#endif /* __FLINTOS_NATIVE_NATIVE_EVENT_RECEIVER_H */
