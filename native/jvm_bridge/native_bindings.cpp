#include "jvm_bridge/native_bindings.hpp"

#include "core/logger.hpp"
#include "core/storage_service.hpp"
#include "hal/board_profile.hpp"
#include "hal/display_driver.hpp"
#include "hal/network_driver.hpp"

namespace flintos {

void NativeBindings::registerAll() {
    Logger::info("Preparing FlintOS native bindings");
    Logger::info("Binding flint.os.SystemInfo");
    Logger::info("Binding flint.os.Logger");
    Logger::info("Binding flint.ui.Screen");
    Logger::info("Binding flint.app.AppManager");
    Logger::info("Binding flint.os.Settings");
    Logger::info("Binding flint.os.UpdateManager");
    Logger::info("Binding flint.os.Storage");
    Logger::info("Binding flint.os.Power");
    Logger::info("Binding flint.ui.Input");
    Logger::info("Binding flint.net.WiFi");
    Logger::info("Binding flint.io.Pin");
    Logger::info("Binding flint.io.I2C");
    Logger::info("Binding flint.io.SPI");
}

const char* NativeBindings::osVersion() const {
    return "0.1.0";
}

const char* NativeBindings::boardName() const {
    static BoardProfile board;
    return board.name();
}

void NativeBindings::screenClear() {
    DisplayDriver display;
    display.clear();
}

void NativeBindings::screenDrawText(const char* text, int x, int y) {
    DisplayDriver display;
    display.drawText(text, x, y);
}

bool NativeBindings::wifiConnect(const char* ssid) {
    NetworkDriver network;
    network.initialize();
    return network.connectWifi(ssid);
}

bool NativeBindings::storageExists(const char* path) const {
    StorageService storage;
    storage.initialize();
    return storage.exists(path);
}

const char* NativeBindings::storageRootPath() const {
    StorageService storage;
    storage.initialize();
    return storage.rootPath();
}

} // namespace flintos
