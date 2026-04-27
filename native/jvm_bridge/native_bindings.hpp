#pragma once

namespace flintos {

class NativeBindings {
public:
    void registerAll();
    const char* osVersion() const;
    const char* boardName() const;
    void screenClear();
    void screenDrawText(const char* text, int x, int y);
    bool wifiConnect(const char* ssid);
    bool storageExists(const char* path) const;
    const char* storageRootPath() const;
};

} // namespace flintos
