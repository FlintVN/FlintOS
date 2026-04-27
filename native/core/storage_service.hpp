#pragma once

namespace flintos {

class StorageService {
public:
    void initialize();
    bool isInitialized() const;
    bool exists(const char* path) const;
    const char* rootPath() const;

private:
    bool initialized_ = false;
};

} // namespace flintos
