#ifndef __FLINTOS_NATIVE_PLATFORM_H
#define __FLINTOS_NATIVE_PLATFORM_H

#include "flint_native.h"
#include "flintos_hal_platform.h"

namespace NativePlatform {
    enum class Kind { Tls };
    struct Resource;

    void fail(FNIEnv *env, const char *message);
    bool result(FNIEnv *env, bool success, const HAL::Platform::Error &error);
    bool text(FNIEnv *env, jstring value, HAL::Platform::Text &output, size_t maxBytes);
    bool range(FNIEnv *env, jbyteArray buffer, jint offset, jint length, size_t maxBytes);

    class Lease {
        Resource *resource;
    public:
        Lease(FNIEnv *env, jint handle, Kind kind);
        ~Lease();
        explicit operator bool() const { return resource != nullptr; }
        void *context() const;
        Lease(const Lease &) = delete;
        void operator=(const Lease &) = delete;
    };

    jint openTls(FNIEnv *env, const HAL::Platform::Text &host, jint port, jint timeoutMs);
    void close(FNIEnv *env, jint handle, Kind kind);
}

#endif /* __FLINTOS_NATIVE_PLATFORM_H */
