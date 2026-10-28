
#ifndef __FLINTOS_DEFAULT_APPS_H
#define __FLINTOS_DEFAULT_APPS_H

#include "flint_dictionary.h"

class DefaultApps {
private:
    DefaultApps(const DefaultApps &) = delete;
    void operator=(const DefaultApps &) = delete;
public:
    static bool add(const char *extName, uint16_t extLen, const char *program);
    static const char *find(const char *extName, uint16_t length = 0xFFFF);
};

#endif /* __FLINTOS_DEFAULT_APPS_H */
