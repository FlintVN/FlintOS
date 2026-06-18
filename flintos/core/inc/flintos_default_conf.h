
#ifndef __FLINTOS_DEFAULT_CONF_H
#define __FLINTOS_DEFAULT_CONF_H

#if __has_include("flintos_conf.h")
#include "flintos_conf.h"
#endif

#ifndef DISPLAY_WIDTH
    #define DISPLAY_WIDTH           240
    #warning "DISPLAY_WIDTH is not defined. Default value will be used"
#endif /* DISPLAY_WIDTH */

#ifndef DISPLAY_HEIGHT
    #define DISPLAY_HEIGHT          320
    #warning "DISPLAY_HEIGHT is not defined. Default value will be used"
#endif /* DISPLAY_HEIGHT */

#ifndef DISPLAY_FREQ
    #define DISPLAY_FREQ            60
    #warning "DISPLAY_FREQ is not defined. Default value will be used"
#endif /* DISPLAY_HEIGHT */

#endif /* __FLINTOS_DEFAULT_CONF_H */
