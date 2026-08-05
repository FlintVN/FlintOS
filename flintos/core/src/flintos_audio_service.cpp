
#include "flint_mutex.h"
#include "flintos_devices.h"
#include "flint_system_api.h"
#include "flintos_default_conf.h"
#include "flintos_audio_service.h"

#define AUDIO_FRAME_BUF_LENGTH      (AUDIO_FRAME_BUF_SIZE / sizeof(int16_t))

static atomic_flag audioLocked = ATOMIC_FLAG_INIT;
static uint8_t volumn = 0;
static int16_t audioBuff[AUDIO_FRAME_NUM * AUDIO_FRAME_BUF_LENGTH];
static volatile uint32_t currentPos = 0;

static void audioLock(void) {
    while(atomic_flag_test_and_set_explicit(&audioLocked, memory_order_acquire))
        FlintAPI::Thread::yield();
}

static void audioUnlock(void) {
    atomic_flag_clear_explicit(&audioLocked, memory_order_release);
}

void AudioSrv::mainTask(void) {
    while(1) {
        uint8_t *frame = (uint8_t *)&audioBuff[currentPos];
        uint32_t bw = 0;
        while(bw < AUDIO_FRAME_BUF_SIZE)
            bw += FDev::Audio::write(&frame[bw], AUDIO_FRAME_BUF_SIZE - bw);
        memset(frame, 0, AUDIO_FRAME_BUF_SIZE);
        currentPos = (currentPos + AUDIO_FRAME_BUF_LENGTH) % LENGTH(audioBuff);
    }
}

uint32_t AudioSrv::open(void) {
    return (currentPos + AUDIO_FRAME_BUF_LENGTH * 2) % LENGTH(audioBuff);
}

uint32_t AudioSrv::write(int32_t *pos, int16_t *frame, uint32_t length) {
    int32_t localPos = *pos;
    if(localPos == currentPos) return 0;
    uint32_t count = 0;
    audioLock();
    for(; (count < length) && (localPos != currentPos); count++) {
        int32_t tmp = audioBuff[localPos] + *frame++;
        if(tmp > 32767) tmp = 32767;
        else if(tmp < -32768) tmp = -32768;
        audioBuff[localPos] = tmp;
        localPos = (localPos + 1) % LENGTH(audioBuff);
    }
    audioUnlock();
    *pos = localPos;
    return count;
}

uint8_t AudioSrv::getVolumn(void) {
    return volumn;
}

void AudioSrv::setVolumn(uint8_t value) {
    volumn = value;
    FDev::Audio::setVolumn(value);
}
