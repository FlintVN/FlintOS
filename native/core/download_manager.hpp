#pragma once

#include <cstddef>
#include <cstdint>

namespace flintos {

class DownloadManager {
public:
    void initialize();
    bool isInitialized() const;
    bool downloadUrl(const char* url, const char* outputPath, uint32_t timeoutMs = 10000);
    const char* downloadText(const char* url, uint32_t timeoutMs = 10000);
    const char* lastError() const;

private:
    static constexpr std::size_t BufferSize = 256;

    bool initialized_ = false;
    char textBuffer_[BufferSize] = {};
    const char* lastError_ = nullptr;
};

} // namespace flintos
