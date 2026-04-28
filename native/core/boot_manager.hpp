#pragma once

#include "core/app_registry.hpp"
#include "core/ota_update_manager.hpp"
#include "core/package_manager.hpp"
#include "core/permission_manager.hpp"
#include "core/service_registry.hpp"
#include "core/settings_store.hpp"
#include "core/storage_service.hpp"
#include "hal/audio_driver.hpp"
#include "hal/board_profile.hpp"
#include "hal/display_driver.hpp"
#include "hal/input_driver.hpp"
#include "hal/network_driver.hpp"
#include "hal/power_manager.hpp"

namespace flintos {

class BootManager {
public:
    void initialize();
    void start();

private:
    BoardProfile board_;
    StorageService storage_;
    DisplayDriver display_;
    AudioDriver audio_;
    InputDriver input_;
    NetworkDriver network_;
    PowerManager power_;
    PermissionManager permissions_;
    AppRegistry apps_;
    SettingsStore settings_;
    PackageManager packages_;
    OTAUpdateManager ota_;
    ServiceRegistry services_;
};

} // namespace flintos
