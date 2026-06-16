
#ifndef __FLINTOS_DEFAULT_CONF_H
#define __FLINTOS_DEFAULT_CONF_H

#if __has_include("flintos_conf.h")
#include "flintos_conf.h"
#endif

#ifndef SCREEN_WIDTH
    #define SCREEN_WIDTH          240
    #warning "SCREEN_WIDTH is not defined. Default value will be used"
#endif /* SCREEN_WIDTH */

#ifndef SCREEN_HEIGHT
    #define SCREEN_HEIGHT         320
    #warning "SCREEN_HEIGHT is not defined. Default value will be used"
#endif /* SCREEN_HEIGHT */

#endif /* __FLINTOS_DEFAULT_CONF_H */
