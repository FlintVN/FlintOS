#pragma once

namespace flintos {

class FlintJvmHost {
public:
    void initialize();
    void launchSystemApp(const char* mainClassName);
};

} // namespace flintos
