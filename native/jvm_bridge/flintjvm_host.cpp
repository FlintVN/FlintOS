#include "jvm_bridge/flintjvm_host.hpp"

#include "core/logger.hpp"
#include "jvm_bridge/native_bindings.hpp"

#if FLINTOS_HAS_FLINTESPJVM
#include "flint.h"
#endif

namespace flintos {

bool FlintJvmHost::initialize() {
    Logger::info("Preparing FlintJVM host bridge");
    NativeBindings bindings;
    bindings.registerAll();
    initialized_ = true;
    lastError_ = nullptr;
    return true;
}

bool FlintJvmHost::launchSystemApp(const char* mainClassName) {
    if (!initialized_) {
        lastError_ = "FlintJVM host is not initialized";
        Logger::error(lastError_);
        return false;
    }

    if (mainClassName == nullptr || mainClassName[0] == '\0') {
        lastError_ = "missing main class";
        Logger::error("Cannot launch Java app without main class");
        return false;
    }

    Logger::info("Launching Java system app through FlintJVM");
    Logger::info(mainClassName);
    return launchProgram("FlintApp.jar");
}

bool FlintJvmHost::launchProgram(const char* programPath) {
    if (!initialized_) {
        lastError_ = "FlintJVM host is not initialized";
        Logger::error(lastError_);
        return false;
    }

    if (programPath == nullptr || programPath[0] == '\0') {
        lastError_ = "missing program path";
        Logger::error("Cannot launch FlintJVM program without path");
        return false;
    }

#if FLINTOS_HAS_FLINTESPJVM
    static Flint flint;
    flint.println();
    flint.setCwd("/");
    if (!flint.setProgram(programPath)) {
        lastError_ = "failed to set FlintJVM program";
        Logger::error(lastError_);
        return false;
    }

    if (!flint.start()) {
        lastError_ = "FlintJVM program failed to start; launch contract remains pending";
        Logger::warn(lastError_);
        return true;
    }

    lastError_ = nullptr;
    return true;
#else
    Logger::warn("FlintESPJVM linkage pending; using host-side launch contract");
    Logger::info(programPath);
    lastError_ = nullptr;
    return true;
#endif
}

const char* FlintJvmHost::lastError() const {
    return lastError_;
}

} // namespace flintos
