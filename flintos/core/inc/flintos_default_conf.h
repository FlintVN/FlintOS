
#ifndef __FLINTOS_DEFAULT_CONF_H
#define __FLINTOS_DEFAULT_CONF_H

#if __has_include("flintos_conf.h")
#include "flintos_conf.h"
#endif

#ifndef DISPLAY_WIDTH
    #define DISPLAY_WIDTH               240
    #warning "DISPLAY_WIDTH is not defined. Default value will be used"
#endif /* DISPLAY_WIDTH */

#ifndef DISPLAY_HEIGHT
    #define DISPLAY_HEIGHT              320
    #warning "DISPLAY_HEIGHT is not defined. Default value will be used"
#endif /* DISPLAY_HEIGHT */

#ifndef DISPLAY_FREQ
    #define DISPLAY_FREQ                60
    #warning "DISPLAY_FREQ is not defined. Default value will be used"
#endif /* DISPLAY_HEIGHT */

#ifndef AUDIO_FRAME_BUF_SIZE
    #define AUDIO_FRAME_BUF_SIZE        256
    #warning "AUDIO_FRAME_BUF_SIZE is not defined. Default value will be used"
#endif /* AUDIO_FRAME_BUF_SIZE */

#ifndef AUDIO_FRAME_NUM
    #define AUDIO_FRAME_NUM             10
    #warning "AUDIO_FRAME_NUM is not defined. Default value will be used"
#endif /* AUDIO_FRAME_NUM */

#ifndef AUDIO_SAMPLE_RATE
    #define AUDIO_SAMPLE_RATE           44100
    #warning "AUDIO_SAMPLE_RATE is not defined. Default value will be used"
#endif /* AUDIO_SAMPLE_RATE */         

#endif /* __FLINTOS_DEFAULT_CONF_H */
