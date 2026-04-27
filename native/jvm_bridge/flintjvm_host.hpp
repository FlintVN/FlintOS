#pragma once

namespace flintos {

class FlintJvmHost {
public:
    bool initialize();
    bool launchSystemApp(const char* mainClassName);
    bool launchProgram(const char* programPath);
    const char* lastError() const;

private:
    bool initialized_ = false;
    const char* lastError_ = nullptr;
};

} // namespace flintos
