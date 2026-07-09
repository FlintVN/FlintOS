
#ifndef __AUDIO_SERVICE_H
#define __AUDIO_SERVICE_H

class AudioSrv {
public:
    static void mainTask(void);
    static uint32_t open(void);
    static uint32_t write(int32_t *pos, int16_t *frame, uint32_t length);
    static uint8_t getVolumn(void);
    static void setVolumn(uint8_t value);
};

#endif /* __AUDIO_SERVICE_H */
