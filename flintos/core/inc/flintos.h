
#ifndef __FLINTOS_H
#define __FLINTOS_H

#include "flint.h"

class FlintOS {
public:
    static void main(void);
    static Flint *newFlint(void);
    static void startup(void);
    static bool open(const char *file);
private:
    FlintOS(const FlintOS &) = delete;
    void operator=(const FlintOS &) = delete;
};

#endif /* __FLINTOS_H */
