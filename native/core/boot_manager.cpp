#include "core/boot_manager.hpp"

#include "core/logger.hpp"
#include "jvm_bridge/flintjvm_host.hpp"

namespace flintos {

void BootManager::initialize() {
    Logger::info("Initializing FlintOS native services");
    // TODO: initialize board profile, storage, display, input, networking, settings, permissions.
}

void BootManager::start() {
    Logger::info("Starting FlintJVM host");
    FlintJvmHost jvm;
    jvm.initialize();
    jvm.launchSystemApp("flintos.launcher.LauncherApp");
}

} // namespace flintos
