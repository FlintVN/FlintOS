#pragma once

#include <cstddef>
#include <cstdint>

namespace flintos {

class TaskManager {
public:
    void initialize();
    bool isInitialized() const;
    int createTask(const char* taskName, uint32_t delayMs = 0, uint32_t timeoutMs = 0);
    void cancelTask(int taskId);
    bool isTaskActive(int taskId) const;
    void sleepMs(uint32_t milliseconds) const;
    const char* lastError() const;

private:
    static constexpr std::size_t MaxTasks = 16;

    struct TaskRecord {
        int id = 0;
        const char* name = nullptr;
        bool active = false;
        uint32_t delayMs = 0;
        uint32_t timeoutMs = 0;
    };

    bool initialized_ = false;
    int nextTaskId_ = 1;
    TaskRecord tasks_[MaxTasks] = {};
    const char* lastError_ = nullptr;
};

} // namespace flintos
