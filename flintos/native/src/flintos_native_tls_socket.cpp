#include <cstring>
#include "flint_array_object.h"
#include "flintos_native_tls_socket.h"
#include "flintos_native_platform.h"

jint NativeTlsSocket_Open(FNIEnv *env, jstring hostValue, jint port, jint timeoutMs) {
    HAL::Platform::Text host;
    if(!NativePlatform::text(env, hostValue, host, 253)) return 0;
    if(host.empty() || strpbrk(host.c_str(), "\r\n /:@") != nullptr || port < 1 || port > 65535 || timeoutMs < 250 || timeoutMs > 20000) {
        NativePlatform::fail(env, "Invalid TLS hostname, port or timeout"); return 0;
    }
    return NativePlatform::openTls(env, host, port, timeoutMs);
}
jint NativeTlsSocket_Read(FNIEnv *env, jint handle, jbyteArray buffer, jint offset, jint length) {
    if(!NativePlatform::range(env, buffer, offset, length, 16384)) return -1;
    NativePlatform::Lease lease(env, handle, NativePlatform::Kind::Tls);
    if(!lease) return -1;
    if(!length) return 0;
    HAL::Platform::Error error;
    int count = 0;
    if(!NativePlatform::result(env, HAL::Platform::tlsRead(static_cast<HAL::Platform::Tls *>(lease.context()), reinterpret_cast<uint8_t *>(buffer->getData())+offset, length, count, error), error)) return -1;
    if(count < -1 || count > length) { NativePlatform::fail(env, "Invalid TLS read result"); return -1; }
    return count;
}
jvoid NativeTlsSocket_Write(FNIEnv *env, jint handle, jbyteArray buffer, jint offset, jint length) {
    if(!NativePlatform::range(env, buffer, offset, length, 16384)) return;
    NativePlatform::Lease lease(env, handle, NativePlatform::Kind::Tls);
    if(!lease || !length) return;
    HAL::Platform::Error error;
    NativePlatform::result(env, HAL::Platform::tlsWrite(static_cast<HAL::Platform::Tls *>(lease.context()), reinterpret_cast<const uint8_t *>(buffer->getData())+offset, length, error), error);
}
jvoid NativeTlsSocket_Close(FNIEnv *env, jint handle) { NativePlatform::close(env, handle, NativePlatform::Kind::Tls); }
