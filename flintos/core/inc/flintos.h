
#ifndef __FLINTOS_H
#define __FLINTOS_H

#include "flint.h"
#include "flintos_process.h"
#include "flintos_event_queue.h"
#include "flint_native_interface.h"

class FlintOS {
public:
    static void main(void);
    static void startup(void);
    static FProcess *newProcess(void);
    static FProcess *open(const char *file, void *args = NULL);
    
    static FList<FProcess> *getProcesses(void);

    static void setHomeApp(FProcess *process);

    static bool isForeground(FProcess *process, bool checkOnly = true);
    static void setForeground(FProcess *process);

    static bool postEvent(const FEvent *event);
private:
    FlintOS(const FlintOS &) = delete;
    void operator=(const FlintOS &) = delete;

    static void lock();
    static void unlock();

    friend jvoid NativeProcess_Close(class FNIEnv *, jobject);
    friend jvoid NativeProcess_Foreground(class FNIEnv *, jobject);
    friend jobjectArray NativeProcess_GetProcesses(class FNIEnv *);
};

#endif /* __FLINTOS_H */
