
#ifndef __FLINTOS_H
#define __FLINTOS_H

#include "flint.h"

class FlintOS {
public:
    static void main(void);
    static Flint *newFlint(void);
private:
    FlintOS(const FlintOS &) = delete;
    void operator=(const FlintOS &) = delete;
};

#endif /* __FLINTOS_H */
