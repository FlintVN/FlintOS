#pragma once

#include <cstddef>

#include "core/app_manifest.hpp"

namespace flintos {

class AppRegistry {
public:
    void initialize();
    void refresh();
    bool isInitialized() const;
    bool validate(const AppManifest& manifest) const;
    const AppManifest* installedApps(std::size_t* count) const;
    const AppManifest* systemApps(std::size_t* count) const;
    const AppManifest* findById(const char* appId) const;

private:
    bool initialized_ = false;
};

} // namespace flintos
