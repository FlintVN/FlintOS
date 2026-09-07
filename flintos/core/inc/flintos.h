
#ifndef __FLINTOS_H
#define __FLINTOS_H

#include "flintos_process.h"
#include "flintos_event_queue.h"

class FlintOS {
public:
    static void main(void);
    static void startup(void);
    static FProcess *newProcess(void);
    static FProcess *open(const char *file);

    static void setHomeApp(FProcess *process);

    static bool isForeground(FProcess *process, bool checkOnly = true);
    static void setForeground(FProcess *process);

    static bool postEvent(const FEvent *event);
private:
    FlintOS(const FlintOS &) = delete;
    void operator=(const FlintOS &) = delete;
};

#endif /* __FLINTOS_H */
