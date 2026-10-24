#ifndef __FLINTOS_HAL_PLATFORM_BUFFER_H
#define __FLINTOS_HAL_PLATFORM_BUFFER_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <limits>
#include <type_traits>

namespace HAL::Platform {
    inline void *bufferAllocate(size_t bytes) noexcept { return malloc(bytes); }
    inline void bufferFree(void *value) noexcept { free(value); }

    // These boundary buffers never call throwing C++ allocation. Failed growth
    // preserves the previous value; callers must check resize/assign/reserve.
    template<class T> class Buffer {
        static_assert(std::is_trivially_copyable<T>::value, "Platform buffers contain plain data");
        T *value = nullptr;
        size_t length = 0, capacity = 0;
    public:
        Buffer() noexcept = default;
        ~Buffer() { bufferFree(value); }
        Buffer(const Buffer &) = delete;
        Buffer &operator=(const Buffer &) = delete;
        size_t size() const noexcept { return length; }
        bool empty() const noexcept { return length == 0; }
        T *data() noexcept { return value; }
        const T *data() const noexcept { return value; }
        T &operator[](size_t index) noexcept { return value[index]; }
        const T &operator[](size_t index) const noexcept { return value[index]; }
        void clear() noexcept { length = 0; }
        bool reserve(size_t count) noexcept {
            if(count <= capacity) return true;
            if(count > std::numeric_limits<size_t>::max() / sizeof(T)) return false;
            auto *next = static_cast<T *>(bufferAllocate(count * sizeof(T)));
            if(!next) return false;
            if(length) memcpy(next, value, length * sizeof(T));
            bufferFree(value); value = next; capacity = count;
            return true;
        }
        bool resize(size_t count) noexcept {
            if(!reserve(count)) return false;
            length = count; return true;
        }
        bool assign(const T *source, size_t count) noexcept {
            if(!source && count) return false;
            if(!resize(count)) return false;
            if(count) memcpy(value, source, count * sizeof(T));
            return true;
        }
    };
    using ByteBuffer = Buffer<uint8_t>;

    class Text {
        Buffer<char> value;
    public:
        size_t size() const noexcept { return value.empty() ? 0 : value.size() - 1; }
        bool empty() const noexcept { return size() == 0; }
        const char *c_str() const noexcept { return value.empty() ? "" : value.data(); }
        const char *data() const noexcept { return c_str(); }
        char *data() noexcept { return value.data(); }
        void clear() noexcept { value.clear(); }
        bool resize(size_t count) noexcept {
            if(!count) { clear(); return true; }
            if(count == std::numeric_limits<size_t>::max() || !value.resize(count + 1)) return false;
            value[count] = 0; return true;
        }
        bool assign(const char *source, size_t count) noexcept {
            if(!source && count) return false;
            if(!resize(count)) return false;
            if(count) memcpy(value.data(), source, count);
            return true;
        }
    };
}

#endif /* __FLINTOS_HAL_PLATFORM_BUFFER_H */
