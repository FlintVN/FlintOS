#ifndef __FLINTOS_HAL_PLATFORM_H
#define __FLINTOS_HAL_PLATFORM_H

#include <stddef.h>
#include <stdint.h>
#include "flintos_hal_platform_buffer.h"

// Board services expose bytes and opaque resources, never JVM or app policy.
// Resource I/O is bounded; cancel requests interruption before final close.
// The caller must drain all resource leases before final close frees a context.
namespace HAL::Platform {
    struct Error { char message[384] = {}; };
    struct Tls;

    bool syncTime(Error &error);

    bool tlsOpen(const char *host, size_t hostLength, uint16_t port, uint32_t timeoutMs, Tls *&tls, Error &error);
    // count is 0 for a bounded timeout, -1 for EOF, positive for bytes read.
    bool tlsRead(Tls *tls, uint8_t *data, size_t length, int &count, Error &error);
    bool tlsWrite(Tls *tls, const uint8_t *data, size_t length, Error &error);
    void tlsCancel(Tls *tls);
    void tlsClose(Tls *tls);
}

#endif /* __FLINTOS_HAL_PLATFORM_H */
