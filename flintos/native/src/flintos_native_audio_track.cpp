
#include <cstdlib>
#include "flint_system_api.h"
#include "flint_array_object.h"
#include "flint_native_common.h"
#include "flintos_default_conf.h"
#include "flintos_audio_service.h"
#include "flintos_native_audio_track.h"

class JAudioTrack : public JObject {
public:
    jint getPos(void) { return getFieldByIndex(0)->getInt32(); }
    jint getSampleRate(void) { return getFieldByIndex(1)->getInt32(); }
    jint getAudioFormat(void) { return getFieldByIndex(2)->getInt32(); }
    jint getChannels(void) { return getFieldByIndex(3)->getInt32(); }
    jbyteArray getBuff(void) { return (jbyteArray)getFieldByIndex(4)->getObj(); }
    jint getBufPos(void) { return getFieldByIndex(5)->getInt32(); }
    jint getSrcPos(void) { return getFieldByIndex(6)->getInt32(); }
    jint getSrcAcc(void) { return getFieldByIndex(7)->getInt32(); }

    void setPos(jint val) { getFieldByIndex(0)->setInt32(val); }
    void setSampleRate(jint val) { getFieldByIndex(1)->setInt32(val); }
    void setBufPos(jint val) { getFieldByIndex(5)->setInt32(val); }
    void setSrcPos(jint val) { getFieldByIndex(6)->setInt32(val); }
    void setSrcAcc(jint val) { getFieldByIndex(7)->setInt32(val); }
private:
    JAudioTrack(const JAudioTrack &) = delete;
    void operator=(const JAudioTrack &) = delete;
};

static bool AudioSrv_Write(FNIEnv *env, JAudioTrack *audioTrack) {
    int16_t *buf = (int16_t *)audioTrack->getBuff()->getData();
    uint32_t len = audioTrack->getBuff()->getLength() >> 1;
    int32_t pos = audioTrack->getPos();
    while(len > 0) {
        if(env->hasTerminateRequest()) return false;
        uint32_t w = AudioSrv::write(&pos, buf, len);
        if(w == 0)
            FlintAPI::Thread::yield();
        else {
            buf += w;
            len -= w;
        }
    }
    audioTrack->setPos(pos);
    return true;
}

static void putPcm8(FNIEnv *env, JAudioTrack *audioTrack, int8_t *data, uint32_t len) {
    uint32_t sampleRate = audioTrack->getSampleRate();
    uint32_t bytePerSample = audioTrack->getChannels();

    int8_t *buf = audioTrack->getBuff()->getData();
    uint32_t bufSz = audioTrack->getBuff()->getLength();
    int32_t bufPos= audioTrack->getBufPos();

    int32_t srcPos = audioTrack->getSrcPos();
    int32_t srcAcc = audioTrack->getSrcAcc();

    while(srcPos < len) {
        uint16_t u16Data = data[srcPos] << 8;
        buf[bufPos++] = (uint8_t)u16Data;
        buf[bufPos++] = (uint8_t)(u16Data >> 8);
        if(bufPos == bufSz) {
            if(!AudioSrv_Write(env, audioTrack)) return;
            bufPos = 0;
        }

        srcAcc += sampleRate;
        if(srcAcc >= AUDIO_SAMPLE_RATE) {
            srcPos += (srcAcc / AUDIO_SAMPLE_RATE) * bytePerSample;
            srcAcc %= AUDIO_SAMPLE_RATE;
        }
    }

    audioTrack->setSrcPos(srcPos % len);
    audioTrack->setBufPos(bufPos);
    audioTrack->setSrcAcc(srcAcc);
}

static void putPcm16(FNIEnv *env, JAudioTrack *audioTrack, int8_t *data, uint32_t len) {
    uint32_t sampleRate = audioTrack->getSampleRate();
    uint32_t bytePerSample = audioTrack->getChannels() << 1;

    int8_t *buf = audioTrack->getBuff()->getData();
    uint32_t bufSz = audioTrack->getBuff()->getLength();
    int32_t bufPos= audioTrack->getBufPos();

    int32_t srcPos = audioTrack->getSrcPos();
    int32_t srcAcc = audioTrack->getSrcAcc();

    if(srcPos == -1) {
        int8_t lastVal = buf[--bufPos];
        while(1) {
            buf[bufPos++] = lastVal;
            buf[bufPos++] = data[0];
            if(bufPos == bufSz) {
                if(!AudioSrv_Write(env, audioTrack)) return;
                bufPos = 0;
            }

            srcAcc += sampleRate;
            if(srcAcc >= AUDIO_SAMPLE_RATE) {
                srcPos += (srcAcc / AUDIO_SAMPLE_RATE) * bytePerSample;
                srcAcc %= AUDIO_SAMPLE_RATE;
                break;
            }
        }
    }

    while(srcPos < len) {
        buf[bufPos++] = data[srcPos + 0];
        if((srcPos + 1) >= len) {
            srcPos = -1;
            break;
        }
        buf[bufPos++] = data[srcPos + 1];
        if(bufPos == bufSz) {
            if(!AudioSrv_Write(env, audioTrack)) return;
            bufPos = 0;
        }

        srcAcc += sampleRate;
        if(srcAcc >= AUDIO_SAMPLE_RATE) {
            srcPos += (srcAcc / AUDIO_SAMPLE_RATE) * bytePerSample;
            srcAcc %= AUDIO_SAMPLE_RATE;
        }
    }

    audioTrack->setSrcPos(srcPos == -1 ? -1 : (srcPos % len));
    audioTrack->setBufPos(bufPos);
    audioTrack->setSrcAcc(srcAcc);
}

static void putPcm(FNIEnv *env, JAudioTrack *audioTrack, int8_t *data, uint32_t len) {
    switch(audioTrack->getAudioFormat()) {
        case 0:
            return putPcm8(env, audioTrack, data, len);
        case 1:
            return putPcm16(env, audioTrack, data, len);
        default:
            return env->throwNew(env->findClass("java/io/IllegalArgumentException"), "Unknown audio format or format not supported");
    }
}

jint NativeAudioTrack_GetFrameSize(FNIEnv *env) {
    return AUDIO_FRAME_BUF_SIZE;
}

jint NativeAudioTrack_GetVolumn(FNIEnv *env) {
    return AudioSrv::getVolumn();
}

jvoid NativeAudioTrack_SetVolumn(FNIEnv *env, jint value) {
    if(value < 0) value = 0;
    else if(value > 100) value = 100;
    AudioSrv::setVolumn(value);
}

jint NativeAudioTrack_Open0(FNIEnv *env, jobject obj) {
    (void)env;
    return AudioSrv::open();
}

jvoid NativeAudioTrack_Write1(FNIEnv *env, jobject obj, jbyteArray b) {
    JAudioTrack *audioTrack = (JAudioTrack *)obj;
    if(audioTrack->getPos() < 0)
        return env->throwNew(env->findClass("java/io/IllegalStateException"), "AudioTrack has not been opened");
    if(b == NULL)
        return env->throwNew(env->findClass("java/lang/NullPointerException"));
    
    putPcm(env, audioTrack, b->getData(), b->getLength());
}

jvoid NativeAudioTrack_Write2(FNIEnv *env, jobject obj, jbyteArray b, jint off, jint len) {
    JAudioTrack *audioTrack = (JAudioTrack *)obj;
    if(audioTrack->getPos() < 0)
        return env->throwNew(env->findClass("java/io/IllegalStateException"), "AudioTrack has not been opened");
    if(b == NULL)
        return env->throwNew(env->findClass("java/lang/NullPointerException"));
    if(!CheckArrayIndexSize(env, b, off, len)) return;

    putPcm(env, audioTrack, &b->getData()[off], len);
}
