#pragma once

#include <cstddef>

namespace flintos {

enum class AppTrustState {
    Trusted,
    Unverified,
};

struct AppManifest {
    const char* id;
    const char* name;
    const char* version;
    const char* type;
    const char* runtime;
    const char* mainClass;
    const char* const* permissions;
    std::size_t permissionCount;
    const char* minOs;
    const char* minJdk;
    const char* checksum;
    AppTrustState trustState;
};

} // namespace flintos
