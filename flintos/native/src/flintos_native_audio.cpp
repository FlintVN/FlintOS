
#include "flint_system_api.h"
#include "flint_array_object.h"
#include "flint_native_common.h"
#include "flintos_default_conf.h"
#include "flintos_native_audio.h"
#include "flintos_audio_service.h"

jint NativeAudio_GetSampleRate(FNIEnv *env) {
    return AUDIO_SAMPLE_RATE;
}

jint NativeAudio_GetChannels(FNIEnv *env) {
    return 1;
}

jint NativeAudio_GetFrameSize(FNIEnv *env) {
    return AUDIO_FRAME_BUF_SIZE;
}

jint NativeAudio_GetVolumn(FNIEnv *env) {
    return AudioSrv::getVolumn();
}

jvoid NativeAudio_SetVolumn(FNIEnv *env, jint value) {
    if(value < 0) value = 0;
    else if(value > 100) value = 100;
    AudioSrv::setVolumn(value);
}

jvoid NativeAudio_Open(FNIEnv *env, jobject obj) {
    (void)env;
    obj->getFieldByIndex(0)->setInt32(AudioSrv::open());
}

jvoid NativeAudio_Write1(FNIEnv *env, jobject obj, jbyteArray b) {
    int32_t pos = obj->getFieldByIndex(0)->getInt32();
    if(pos < 0)
        return env->throwNew(env->findClass("java/io/IOException"), "Audio has not been opened");
    if(b == NULL)
        return env->throwNew(env->findClass("java/lang/NullPointerException"));
    int16_t *buf = (int16_t *)((jbyteArray)b)->getData();
    uint32_t sz = ((jbyteArray)b)->getLength();
    uint32_t len = sz / sizeof(int16_t);
    while(len > 0) {
        uint32_t w = AudioSrv::write(&pos, buf, len);
        if(w == 0)
            FlintAPI::Thread::yield();
        else {
            buf += w;
            len -= w;
        }
    }
    obj->getFieldByIndex(0)->setInt32(pos);
}

jvoid NativeAudio_Write2(FNIEnv *env, jobject obj, jbyteArray b, jint off, jint len) {
    int32_t pos = obj->getFieldByIndex(0)->getInt32();
    if(pos < 0)
        return env->throwNew(env->findClass("java/io/IOException"), "Audio has not been opened");
    if(b == NULL)
        return env->throwNew(env->findClass("java/lang/NullPointerException"));

    if(!CheckArrayIndexSize(env, b, off, len)) return;
    if((off & 1) || (len & 1))
        return env->throwNew(
            env->findClass("java/lang/IllegalArgumentException"),
            "PCM S16LE offset and length must be even"
        );

    int16_t *buf = (int16_t *)(((jbyteArray)b)->getData() + off);
    uint32_t samples = (uint32_t) len / sizeof(int16_t);

    while(samples > 0) {
        uint32_t w = AudioSrv::write(&pos, buf, samples);
        if(w == 0)
            FlintAPI::Thread::yield();
        else {
            buf += w;
            samples -= w;
        }
    }
    obj->getFieldByIndex(0)->setInt32(pos);
}

jvoid NativeAudio_Close(FNIEnv *env, jobject obj) {
    obj->getFieldByIndex(0)->setInt32(-1);
}
