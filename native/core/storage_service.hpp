#pragma once

namespace flintos {

class StorageService {
public:
    void initialize();
    bool isInitialized() const;
    bool exists(const char* path) const;
    const char* rootPath() const;
    const char* lastError() const;

private:
    bool initialized_ = false;
    bool mounted_ = false;
    const char* lastError_ = nullptr;
};

} // namespace flintos
