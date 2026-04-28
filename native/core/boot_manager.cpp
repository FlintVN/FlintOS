#include "core/boot_manager.hpp"

#include "core/logger.hpp"
#include "jvm_bridge/flintjvm_host.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace flintos {
namespace {

TaskHandle_t bootButtonMusicTask = nullptr;

void watchBootButtonForMusic(void*) {
    InputDriver input;
    AudioDriver audio;
    input.initialize();

    bool wasPressed = input.isBootButtonPressed();
    while (true) {
        const bool pressed = input.isBootButtonPressed();
        if (pressed && !wasPressed) {
            Logger::info("BOOT0 pressed, starting music player");
            if (!audio.startMusicPlayer()) {
                Logger::warn("Music player start failed");
            }
        }
        wasPressed = pressed;
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void startBootButtonMusicWatcher() {
    if (bootButtonMusicTask != nullptr) {
        return;
    }
    if (xTaskCreate(watchBootButtonForMusic, "boot0_music", 3072, nullptr, tskIDLE_PRIORITY + 1, &bootButtonMusicTask) != pdPASS) {
        bootButtonMusicTask = nullptr;
        Logger::warn("BOOT0 music watcher start failed");
    }
}

} // namespace


void BootManager::initialize() {
    Logger::info("Initializing FlintOS native services");
    Logger::info(board_.name());
    storage_.initialize();
    display_.clear();
    audio_.playBootSound();
    display_.playBootAnimation();
    input_.initialize();
    network_.initialize();
    power_.initialize();
    settings_.initialize();
    apps_.initialize();
    packages_.initialize();
    ota_.initialize();
    services_.initialize(permissions_, apps_, settings_, packages_, ota_);
}

void BootManager::start() {
    Logger::info("FLINTOS_TEST_START boot_smoke");

    if (!services_.isInitialized() || !apps_.isInitialized() || !storage_.isInitialized() || !input_.isInitialized() || !network_.isInitialized() || !power_.isInitialized()) {
        Logger::error("FLINTOS_TEST_FAIL boot_smoke native_services_not_ready");
        Logger::info("FLINTOS_TEST_END boot_smoke");
        return;
    }

    const AppManifest* launcher = apps_.findById("flintos.launcher");
    if (launcher == nullptr) {
        Logger::error("FLINTOS_TEST_FAIL boot_smoke launcher_manifest_missing");
        Logger::info("FLINTOS_TEST_END boot_smoke");
        return;
    }

    if (!permissions_.hasPermission(*launcher, "app.launch") || !permissions_.hasPermission(*launcher, "system.info")) {
        Logger::error("FLINTOS_TEST_FAIL boot_smoke launcher_permission_denied");
        Logger::info("FLINTOS_TEST_END boot_smoke");
        return;
    }

    Logger::info("Starting FlintJVM host");
    FlintJvmHost jvm;
    if (!jvm.initialize()) {
        Logger::error("FLINTOS_TEST_FAIL boot_smoke jvm_init_failed");
        Logger::info("FLINTOS_TEST_END boot_smoke");
        return;
    }

    if (!jvm.launchSystemApp(launcher->mainClass)) {
        Logger::warn(jvm.lastError());
        Logger::warn("FLINTOS_TEST_FAIL boot_smoke jvm_launch_pending");
        Logger::info("FLINTOS_TEST_END boot_smoke");
        return;
    }

    if (input_.isBootButtonPressed()) {
        Logger::info("BOOT0 held, starting music player");
        if (!audio_.startMusicPlayer()) {
            Logger::warn("Music player start failed");
        }
    }
    startBootButtonMusicWatcher();

    Logger::info("FLINTOS_TEST_PASS boot_smoke");
    Logger::info("FLINTOS_TEST_END boot_smoke");
}

} // namespace flintos
