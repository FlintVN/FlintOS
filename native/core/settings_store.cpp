#include "core/settings_store.hpp"

#include <cstring>

#include "core/logger.hpp"

namespace flintos {

namespace {

bool equals(const char* left, const char* right) {
    return left != nullptr && right != nullptr && std::strcmp(left, right) == 0;
}

} // namespace

void SettingsStore::initialize() {
    Logger::info("Initializing FlintOS settings store");
    initialized_ = true;
}

bool SettingsStore::isInitialized() const {
    return initialized_;
}

const char* SettingsStore::getString(const char* key, const char* fallback) const {
    if (!initialized_ || key == nullptr || key[0] == '\0') {
        return fallback;
    }

    if (equals(key, "os.version")) {
        return "0.1.0";
    }

    if (equals(key, "os.name")) {
        return "FlintOS";
    }

    return fallback;
}

} // namespace flintos
