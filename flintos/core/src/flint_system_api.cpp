
#include "flintos_devices.h"
#include "flint_system_api.h"

const char *FlintAPI::System::getClassPath(uint32_t index) {
    static const char *jars[] = {
        "/lib/java.base.jar",
        "/lib/flint.net.jar",
        "/lib/flint.drawing.jar",
    };
    if(index < LENGTH(jars))
        return jars[index];
    return NULL;        
}
