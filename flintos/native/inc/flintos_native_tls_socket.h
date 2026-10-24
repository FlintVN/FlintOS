#ifndef __FLINTOS_NATIVE_TLS_SOCKET_H
#define __FLINTOS_NATIVE_TLS_SOCKET_H

#include "flint_native.h"

jint NativeTlsSocket_Open(FNIEnv *env, jstring host, jint port, jint timeoutMs);
jint NativeTlsSocket_Read(FNIEnv *env, jint handle, jbyteArray buffer, jint offset, jint length);
jvoid NativeTlsSocket_Write(FNIEnv *env, jint handle, jbyteArray buffer, jint offset, jint length);
jvoid NativeTlsSocket_Close(FNIEnv *env, jint handle);

inline constexpr NativeMethod tlsSocketMethods[] = {
    NATIVE_METHOD("open", "(Ljava/lang/String;II)I", NativeTlsSocket_Open),
    NATIVE_METHOD("read", "(I[BII)I", NativeTlsSocket_Read),
    NATIVE_METHOD("write", "(I[BII)V", NativeTlsSocket_Write),
    NATIVE_METHOD("close", "(I)V", NativeTlsSocket_Close),
};

#endif /* __FLINTOS_NATIVE_TLS_SOCKET_H */
