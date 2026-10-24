#include <atomic>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <new>
#include "sdkconfig.h"
#if !defined(CONFIG_MBEDTLS_HAVE_TIME_DATE) || CONFIG_MBEDTLS_HAVE_TIME_DATE != 1
#error "FlintOS TLS requires CONFIG_MBEDTLS_HAVE_TIME_DATE=y"
#endif
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "lwip/sockets.h"
#include "esp_tls.h"
#include "esp_crt_bundle.h"
#include "esp_netif_sntp.h"
#include "esp_timer.h"
#include "esp_heap_caps.h"
#include "mbedtls/ssl.h"
#include "mbedtls/x509_crt.h"
#include "flintos_hal_platform.h"

// ESP-IDF's cross-signed CA callback constructs trust anchors from bundle keys,
// without DER or validity dates. Only these synthetic anchors have no date
// constraints; every certificate received from the peer retains all its flags.
extern "C" int esp_crt_verify_callback(void *, mbedtls_x509_crt *, int, uint32_t *);
static int verifyBundle(void *ctx, mbedtls_x509_crt *crt, int depth, uint32_t *flags) {
    if(depth > 0 && crt->raw.p == nullptr && crt->raw.len == 0 &&
       crt->tbs.p == nullptr && crt->tbs.len == 0 && crt->version == 3 &&
       crt->subject_raw.p != nullptr && crt->issuer_raw.p == nullptr &&
       crt->valid_from.year == 0 && crt->valid_to.year == 0)
        *flags &= ~(MBEDTLS_X509_BADCERT_EXPIRED | MBEDTLS_X509_BADCERT_FUTURE);
    return esp_crt_verify_callback(ctx, crt, depth, flags);
}
static esp_err_t attachBundle(void *conf) {
    esp_err_t result = esp_crt_bundle_attach(conf);
    if(result == ESP_OK && conf)
        mbedtls_ssl_conf_verify(static_cast<mbedtls_ssl_config *>(conf), verifyBundle, nullptr);
    return result;
}

namespace HAL::Platform {
struct Tls {
    esp_tls_t *context = nullptr;
    SemaphoreHandle_t lock = nullptr;
    std::atomic<bool> cancelled{false};
    int socket = -1;
};

namespace {
constexpr size_t MAX_TLS_PACKET = 16384;

bool fail(Error &error, const char *format, ...) {
    va_list args; va_start(args, format);
    vsnprintf(error.message, sizeof(error.message), format, args);
    va_end(args); return false;
}
bool take(SemaphoreHandle_t lock, Error &error) {
    return lock && xSemaphoreTake(lock, pdMS_TO_TICKS(1000)) == pdTRUE
        ? true : fail(error, "Platform resource is busy");
}
bool clockValid() { return time(nullptr) >= 1704067200; }
}

bool syncTime(Error &error) {
    if(clockValid()) return true;
    static std::atomic<bool> started{false};
    bool expected = false;
    if(started.compare_exchange_strong(expected, true)) {
        esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG("pool.ntp.org");
        auto result = esp_netif_sntp_init(&config);
        if(result != ESP_OK && result != ESP_ERR_INVALID_STATE) {
            started = false; return fail(error, "Cannot start network time sync");
        }
    }
    for(int i = 0; i < 20; ++i) {
        // SNTP uses a binary semaphore: another caller can consume the signal.
        // The wall clock is shared, so success does not require owning that signal.
        if(clockValid()) return true;
        auto result = esp_netif_sntp_sync_wait(pdMS_TO_TICKS(1000));
        // A concurrent initializer (or external SNTP setup without a wait
        // semaphore) may not be ready yet. Preserve a bounded real-time wait.
        if(result == ESP_ERR_INVALID_STATE) vTaskDelay(pdMS_TO_TICKS(1000));
        if(clockValid()) return true;
    }
    return fail(error, "Network time is unavailable");
}

bool tlsOpen(const char *host, size_t hostLength, uint16_t port, uint32_t timeoutMs, Tls *&tls, Error &error) {
    tls = nullptr;
    if(!host || hostLength < 1 || hostLength > 253 || strlen(host) != hostLength ||
       strpbrk(host, "\r\n /:@") || !port || timeoutMs < 250 || timeoutMs > 20000)
        return fail(error, "TLS requires a hostname and valid port/timeout");
    if(!syncTime(error)) return false;
    auto *resource = new(std::nothrow) Tls;
    if(!resource) return fail(error, "Cannot allocate TLS resource");
    resource->context = esp_tls_init(); resource->lock = xSemaphoreCreateMutex();
    if(!resource->context || !resource->lock) { tlsClose(resource); return fail(error, "Cannot allocate TLS client"); }
    esp_tls_cfg_t config = {}; config.crt_bundle_attach = attachBundle; config.timeout_ms = timeoutMs;
    // Capture only this operation's error and heap facts; do not change the
    // global logger or log levels used by other applications.
    size_t internalFree = heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    size_t internalLargest = heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    size_t externalFree = heap_caps_get_free_size(MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if(esp_tls_conn_new_sync(host, hostLength, port, &config, resource->context) != 1) {
        esp_tls_error_handle_t handle; int tlsCode = 0, flags = 0; esp_err_t result = ESP_FAIL;
        if(esp_tls_get_error_handle(resource->context, &handle) == ESP_OK)
            result = esp_tls_get_and_clear_last_error(handle, &tlsCode, &flags);
        // ESP-TLS omits verification flags when peer certificates are not retained.
        auto *ssl = static_cast<mbedtls_ssl_context *>(esp_tls_get_ssl_context(resource->context));
        if(ssl) flags = mbedtls_ssl_get_verify_result(ssl);
        fail(error, "TLS: %s/%d verify=0x%x time=%lld RAM=%u/%u PSRAM=%u",
             esp_err_to_name(result), tlsCode, (unsigned)flags, (long long)time(nullptr), (unsigned)internalFree,
             (unsigned)internalLargest, (unsigned)externalFree);
        tlsClose(resource); return false;
    }
    if(esp_tls_get_conn_sockfd(resource->context, &resource->socket) != ESP_OK || resource->socket < 0) {
        tlsClose(resource); return fail(error, "TLS socket is unavailable");
    }
    timeval receiveTimeout = {0, 500000};
    if(setsockopt(resource->socket, SOL_SOCKET, SO_RCVTIMEO, &receiveTimeout, sizeof(receiveTimeout)) != 0 ||
       setsockopt(resource->socket, SOL_SOCKET, SO_SNDTIMEO, &receiveTimeout, sizeof(receiveTimeout)) != 0) {
        tlsClose(resource); return fail(error, "Cannot configure bounded TLS I/O");
    }
    tls = resource; return true;
}
bool tlsRead(Tls *tls, uint8_t *data, size_t length, int &count, Error &error) {
    count = 0;
    if(!tls || (!data && length) || length > MAX_TLS_PACKET) return fail(error, "Invalid TLS byte range");
    if(!take(tls->lock, error)) return false;
    if(tls->cancelled) { xSemaphoreGive(tls->lock); return fail(error, "TLS connection cancelled"); }
    auto read = length ? esp_tls_conn_read(tls->context, data, length) : 0;
    bool cancelled = tls->cancelled; xSemaphoreGive(tls->lock);
    if(cancelled) return fail(error, "TLS connection cancelled");
    if(!length || read == ESP_TLS_ERR_SSL_WANT_READ || read == ESP_TLS_ERR_SSL_WANT_WRITE || read == MBEDTLS_ERR_SSL_TIMEOUT) return true;
    if(read < 0) return fail(error, "TLS read failed");
    count = read == 0 ? -1 : read; return true;
}
bool tlsWrite(Tls *tls, const uint8_t *data, size_t length, Error &error) {
    if(!tls || (!data && length) || length > MAX_TLS_PACKET) return fail(error, "Invalid TLS byte range");
    if(!take(tls->lock, error)) return false;
    auto deadline = esp_timer_get_time() + 20000000; size_t sent = 0;
    while(sent < length && !tls->cancelled && esp_timer_get_time() < deadline) {
        auto count = esp_tls_conn_write(tls->context, data + sent, length - sent);
        if(count == ESP_TLS_ERR_SSL_WANT_READ || count == ESP_TLS_ERR_SSL_WANT_WRITE) { vTaskDelay(pdMS_TO_TICKS(1)); continue; }
        if(count <= 0) break;
        sent += count;
    }
    bool cancelled = tls->cancelled; xSemaphoreGive(tls->lock);
    return !cancelled && sent == length ? true : fail(error, "TLS write failed or interrupted");
}
void tlsCancel(Tls *tls) {
    if(tls) { tls->cancelled = true; if(tls->socket >= 0) shutdown(tls->socket, SHUT_RDWR); }
}
void tlsClose(Tls *tls) {
    if(!tls) return;
    tlsCancel(tls);
    if(tls->context) esp_tls_conn_destroy(tls->context);
    if(tls->lock) vSemaphoreDelete(tls->lock);
    delete tls;
}
}
