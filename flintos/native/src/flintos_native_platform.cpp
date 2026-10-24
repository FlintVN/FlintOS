#include <atomic>
#include <climits>
#include <cstring>
#include <new>
#include "flint.h"
#include "flint_execution.h"
#include "flint_mutex.h"
#include "flint_system_api.h"
#include "flintos_native_platform.h"

namespace NativePlatform {
struct Resource {
    jint id = 0;
    Kind kind;
    Flint *owner;
    Hook *hook = nullptr;
    std::atomic<void *> value{nullptr};
    std::atomic<unsigned> refs{1}; // creation owns the first lease
    std::atomic<bool> closing{false};
    bool ready = false; // protected by tableLock
    Resource(Kind kind, Flint *owner) : kind(kind), owner(owner) { }
};

namespace {
FMutex tableLock;
Resource *sessions[4] = {};
unsigned sessionCount = 0;
jint nextId = 0;

Flint *process(FNIEnv *env) { return static_cast<FExec *>(env)->getFlint(); }
void erase(Resource *resource) {
    for(auto &slot : sessions) if(slot == resource) slot = nullptr;
}
void cancel(Resource *resource) {
    void *value = resource->value.load();
    if(!value) return;
    switch(resource->kind) {
        case Kind::Tls: HAL::Platform::tlsCancel(static_cast<HAL::Platform::Tls *>(value)); break;
    }
}
void destroy(Resource *resource) {
    cancel(resource);
    // Provider operations have finite deadlines; cancel requests interruption.
    // Never destroy a context while a native call still holds a reference.
    while(resource->refs.load()) {
        cancel(resource); // also covers an open operation finishing during close
        FlintAPI::Thread::sleep(10);
    }
    void *value = resource->value.load();
    if(value) switch(resource->kind) {
        case Kind::Tls: HAL::Platform::tlsClose(static_cast<HAL::Platform::Tls *>(value)); break;
    }
    // An erased handle still consumes quota until its physical context is gone.
    tableLock.lock();
    --sessionCount;
    tableLock.unlock();
    delete resource;
}
void shutdown(void *argument) {
    auto *resource = static_cast<Resource *>(argument);
    tableLock.lock();
    resource->closing = true;
    erase(resource);
    tableLock.unlock();
    // The JVM invokes and frees this hook while holding its process lock.
    destroy(resource);
}
void detach(Resource *resource) {
    if(resource->hook) {
        resource->owner->removeShutdownHook(resource->hook);
        resource->owner->free(resource->hook);
        resource->hook = nullptr;
    }
}
Resource *reserve(FNIEnv *env, Kind kind) {
    Flint *owner = process(env);
    auto *resource = new(std::nothrow) Resource(kind, owner);
    if(!resource) { fail(env, "Cannot allocate platform resource"); return nullptr; }
    // Match shutdown's process-lock -> table-lock order. Attach before publish.
    owner->lock();
    resource->hook = owner->addShutdownHook(static_cast<FExec *>(env), resource, shutdown);
    if(!resource->hook) { owner->unlock(); delete resource; return nullptr; }
    tableLock.lock();
    Resource **available = nullptr;
    if(sessionCount < 4) for(auto &slot : sessions) if(!slot) { available = &slot; break; }
    if(available && nextId < INT_MAX) {
        resource->id = ++nextId;
        *available = resource;
        ++sessionCount;
    }
    tableLock.unlock();
    if(!resource->id) {
        detach(resource);
        owner->unlock();
        delete resource;
        fail(env, "Platform resource quota exhausted");
        return nullptr;
    }
    owner->unlock();
    return resource;
}
jint finish(FNIEnv *env, Resource *resource, bool success, const HAL::Platform::Error &error) {
    Flint *owner = resource->owner;
    owner->lock();
    tableLock.lock();
    bool publish = success && !env->hasTerminateRequest() && !resource->closing.load();
    bool cleanupOwner = !publish && !resource->closing.load();
    if(publish) resource->ready = true;
    else if(cleanupOwner) { resource->closing = true; erase(resource); }
    jint id = publish ? resource->id : 0;
    tableLock.unlock();
    if(cleanupOwner) detach(resource);
    resource->refs--;
    owner->unlock();
    if(cleanupOwner) destroy(resource);
    if(!success) result(env, false, error);
    else if(!publish && !env->hasTerminateRequest()) fail(env, "Platform resource closed during open");
    return id;
}
}

void fail(FNIEnv *env, const char *message) {
    env->throwNew(env->findClass("java/io/IOException"), "%s", message);
}
bool result(FNIEnv *env, bool success, const HAL::Platform::Error &error) {
    if(!success) env->throwNew(env->findClass("java/io/IOException"), "%.*s", (int)sizeof(error.message)-1,
        error.message[0] ? error.message : "Platform operation failed");
    return success;
}
bool text(FNIEnv *env, jstring value, HAL::Platform::Text &output, size_t maxBytes) {
    if(!value) { env->throwNew(env->findClass("java/lang/NullPointerException")); return false; }
    output.clear();
    auto next = [&](uint32_t &index, uint32_t &c) {
        c = value->getCharAt(index++);
        if(c >= 0xD800 && c <= 0xDBFF) {
            if(index >= value->getLength()) { fail(env, "Invalid Unicode string"); return false; }
            uint32_t low = value->getCharAt(index++);
            if(low < 0xDC00 || low > 0xDFFF) { fail(env, "Invalid Unicode string"); return false; }
            c = 0x10000 + ((c-0xD800)<<10) + low-0xDC00;
        } else if(c >= 0xDC00 && c <= 0xDFFF) { fail(env, "Invalid Unicode string"); return false; }
        if(!c) { fail(env, "String contains NUL"); return false; }
        return true;
    };
    size_t length = 0;
    for(uint32_t i = 0; i < value->getLength();) {
        uint32_t c;
        if(!next(i, c)) return false;
        size_t count = c < 0x80 ? 1 : c < 0x800 ? 2 : c < 0x10000 ? 3 : 4;
        if(length > maxBytes || count > maxBytes-length) { fail(env, "UTF-8 string exceeds byte limit"); return false; }
        length += count;
    }
    if(!output.resize(length)) { fail(env, "Cannot allocate UTF-8 text"); return false; }
    size_t position = 0;
    for(uint32_t i = 0; i < value->getLength();) {
        uint32_t c;
        if(!next(i, c)) { output.clear(); return false; }
        if(c < 0x80) output.data()[position++] = c;
        else if(c < 0x800) { output.data()[position++] = 0xC0|(c>>6); output.data()[position++] = 0x80|(c&63); }
        else if(c < 0x10000) { output.data()[position++] = 0xE0|(c>>12); output.data()[position++] = 0x80|((c>>6)&63); output.data()[position++] = 0x80|(c&63); }
        else { output.data()[position++] = 0xF0|(c>>18); output.data()[position++] = 0x80|((c>>12)&63); output.data()[position++] = 0x80|((c>>6)&63); output.data()[position++] = 0x80|(c&63); }
    }
    return true;
}
bool buffer(FNIEnv *env, jbyteArray value, size_t maxBytes) {
    if(!value) { env->throwNew(env->findClass("java/lang/NullPointerException")); return false; }
    if(strcmp(value->getTypeName(), "[B") != 0) { env->throwNew(env->findClass("java/lang/IllegalArgumentException"), "Expected byte array"); return false; }
    if(value->getLength() > maxBytes) { fail(env, "Byte buffer exceeds limit"); return false; }
    return true;
}
bool range(FNIEnv *env, jbyteArray value, jint offset, jint length, size_t maxBytes) {
    if(!buffer(env, value, UINT32_MAX)) return false;
    uint32_t size = value->getLength();
    if(offset < 0 || length < 0 || (uint32_t)offset > size || (uint32_t)length > size-(uint32_t)offset || (size_t)length > maxBytes) {
        env->throwNew(env->findClass("java/lang/IndexOutOfBoundsException")); return false;
    }
    return true;
}

Lease::Lease(FNIEnv *env, jint handle, Kind kind) : resource(nullptr) {
    Flint *owner = process(env);
    tableLock.lock();
    auto acquire = [&](Resource *candidate) {
        if(candidate && candidate->id == handle && candidate->owner == owner && candidate->kind == kind && candidate->ready && !candidate->closing.load()) {
            candidate->refs++;
            resource = candidate;
        }
    };
    for(auto *candidate : sessions) acquire(candidate);
    tableLock.unlock();
    if(!resource) fail(env, "Platform handle is closed, foreign or of another type");
}
Lease::~Lease() { if(resource) resource->refs--; }
void *Lease::context() const { return resource ? resource->value.load() : nullptr; }

jint openTls(FNIEnv *env, const HAL::Platform::Text &host, jint port, jint timeoutMs) {
    auto *resource = reserve(env, Kind::Tls);
    if(!resource) return 0;
    HAL::Platform::Tls *context = nullptr;
    HAL::Platform::Error error;
    bool success = HAL::Platform::tlsOpen(host.c_str(), host.size(), port, timeoutMs, context, error);
    resource->value = context;
    return finish(env, resource, success && context, error);
}
void close(FNIEnv *env, jint handle, Kind kind) {
    Flint *owner = process(env);
    Resource *resource = nullptr;
    bool invalid = false;
    owner->lock();
    tableLock.lock();
    auto take = [&](Resource *candidate) {
        if(candidate && candidate->id == handle) {
            if(candidate->owner != owner || candidate->kind != kind) invalid = true;
            else { resource = candidate; resource->closing = true; }
        }
    };
    for(auto *candidate : sessions) take(candidate);
    if(resource) erase(resource);
    tableLock.unlock();
    if(resource) detach(resource);
    owner->unlock();
    if(resource) destroy(resource);
    else if(invalid) fail(env, "Cannot close foreign platform handle or another resource type");
    // Already closed handles are harmless; IDs are never reused.
}
}
