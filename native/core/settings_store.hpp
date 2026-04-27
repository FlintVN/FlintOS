#pragma once

namespace flintos {

class SettingsStore {
public:
    void initialize();
    bool isInitialized() const;
    const char* getString(const char* key, const char* fallback) const;

private:
    bool initialized_ = false;
};

} // namespace flintos
