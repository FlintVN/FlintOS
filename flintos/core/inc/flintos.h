
#ifndef __FLINTOS_H
#define __FLINTOS_H

#include "flint.h"

class FProcess : public ListNode, public Flint {
public:
    FProcess(void);
};

class FlintOS {
public:
    static void main(void);
    static void startup(void);
    static FProcess *newProcess(void);
    static FProcess *open(const char *file);

    static void setHomeApp(FProcess *process);

    static bool isForeground(FProcess *process);
    static void setForeground(FProcess *process);
private:
    FlintOS(const FlintOS &) = delete;
    void operator=(const FlintOS &) = delete;
};

#endif /* __FLINTOS_H */
