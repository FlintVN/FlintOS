#include "core/task_manager.hpp"

#if __has_include("freertos/FreeRTOS.h")
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#define FLINTOS_HAS_FREERTOS 1
#else
#define FLINTOS_HAS_FREERTOS 0
#endif

namespace flintos {

namespace {

bool isPresent(const char* value) {
    return value != nullptr && value[0] != '\0';
}

} // namespace

void TaskManager::initialize() {
    initialized_ = true;
    nextTaskId_ = 1;
    lastError_ = nullptr;
    for (TaskRecord& task : tasks_) {
        task = {};
    }
}

bool TaskManager::isInitialized() const {
    return initialized_;
}

int TaskManager::createTask(const char* taskName, uint32_t delayMs, uint32_t timeoutMs) {
    if (!initialized_) {
        lastError_ = "task manager is not initialized";
        return 0;
    }

    if (!isPresent(taskName)) {
        lastError_ = "missing task name";
        return 0;
    }

    for (TaskRecord& task : tasks_) {
        if (!task.active) {
            task.id = nextTaskId_++;
            task.name = taskName;
            task.active = true;
            task.delayMs = delayMs;
            task.timeoutMs = timeoutMs;
            lastError_ = nullptr;
            return task.id;
        }
    }

    lastError_ = "task table is full";
    return 0;
}

void TaskManager::cancelTask(int taskId) {
    for (TaskRecord& task : tasks_) {
        if (task.id == taskId) {
            task.active = false;
            return;
        }
    }
}

bool TaskManager::isTaskActive(int taskId) const {
    for (const TaskRecord& task : tasks_) {
        if (task.id == taskId) {
            return task.active;
        }
    }
    return false;
}

void TaskManager::sleepMs(uint32_t milliseconds) const {
#if FLINTOS_HAS_FREERTOS
    vTaskDelay(pdMS_TO_TICKS(milliseconds));
#else
    (void)milliseconds;
#endif
}

const char* TaskManager::lastError() const {
    return lastError_;
}

} // namespace flintos
