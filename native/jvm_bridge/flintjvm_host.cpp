#include "jvm_bridge/flintjvm_host.hpp"

#include "core/logger.hpp"

namespace flintos {

void FlintJvmHost::initialize() {
    Logger::info("TODO: initialize FlintJVM runtime and native bindings");
}

void FlintJvmHost::launchSystemApp(const char* mainClassName) {
    (void)mainClassName;
    Logger::info("TODO: launch Java system app through FlintJVM");
}

} // namespace flintos
