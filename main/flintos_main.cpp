#include "core/boot_manager.hpp"
#include "core/logger.hpp"

extern "C" void app_main(void) {
    flintos::Logger::info("FlintOS native boot starting");

    flintos::BootManager boot;
    boot.initialize();
    boot.start();
}
