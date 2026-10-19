
#define MINIMP3_IMPLEMENTATION
#define MINIMP3_ONLY_MP3
#include "minimp3.h"

#include "flint.h"
#include <algorithm>
#include "flint_native_common.h"
#include "flintos_native_mp3_decoder.h"

class JMp3Decoder : public JObject {
public:
    jint getHandle(void) { return getFieldByIndex(0)->getInt32(); }
    jbyteArray getBuf(void) { return (jbyteArray)getFieldByIndex(1)->getObj(); }
    jint getOff(void) { return getFieldByIndex(2)->getInt32(); }
    jint getLen(void) { return getFieldByIndex(3)->getInt32(); }

    void setHandle(jint val) { getFieldByIndex(0)->setInt32(val); }
    void setOff(jint val) { getFieldByIndex(2)->setInt32(val); }
    void setLen(jint val) { getFieldByIndex(3)->setInt32(val); }
private:
    JMp3Decoder(const JMp3Decoder &) = delete;
    void operator=(const JMp3Decoder &) = delete;
};

struct flint_mp3dec_t : public mp3dec_t {
    uint32_t mp3Off;
    uint32_t mp3Size;
    uint8_t mp3Buf[4 * 1024];

    uint32_t pcmOff;
    uint32_t pcmSize;
    mp3d_sample_t pcmBuf[MINIMP3_MAX_SAMPLES_PER_FRAME];
};

static flint_mp3dec_t *getMp3DecFromHook(jint hook) {
    return (flint_mp3dec_t *)((Hook *)hook)->getHandle();
}

jint NativeMp3Decoder_Init(FNIEnv *env) {
    FExec *exec = (FExec *)env;
    Flint *flint = exec->getFlint();

    flint_mp3dec_t *mp3d = (flint_mp3dec_t *)FlintAPI::System::malloc(sizeof(flint_mp3dec_t));
    if (mp3d == NULL) {
        env->throwNew(env->findClass("java/lang/OutOfMemoryError"));
        return -1;
    }

    mp3dec_init(mp3d);
    mp3d->mp3Off = 0;
    mp3d->mp3Size = 0;
    mp3d->pcmOff = 0;
    mp3d->pcmSize = 0;

    Hook *hook = flint->addShutdownHook(exec, mp3d, FlintAPI::System::free);
    if (hook == NULL) {
        FlintAPI::System::free(mp3d);
        return -1;
    }

    return (jint)hook;
}

jint NativeMp3Decoder_Decode(FNIEnv *env, jobject obj, jbyteArray b, jint off, jint len) {
    if (!CheckArrayIndexSize(env, b, off, len)) return -1;
    mp3dec_frame_info_t info;
    JMp3Decoder *decoder = (JMp3Decoder *)obj;
    int32_t hook = decoder->getHandle();
    if (hook == -1) {
        env->throwNew(env->findClass("java/lang/IllegalStateException"), "The decoder has been closed");
        return -1;
    }
    flint_mp3dec_t *mp3d = getMp3DecFromHook(hook);
    int8_t *outBuf = b->getData();
    int32_t inOff = decoder->getOff();
    int32_t inLen = decoder->getLen();
    int32_t remaining = len;

    while (remaining) {
        if (mp3d->pcmSize > 0) {
            uint32_t size = std::min<uint32_t>(mp3d->pcmSize, remaining);
            memcpy(outBuf + off, (uint8_t *)mp3d->pcmBuf + mp3d->pcmOff, size);
            off += size;
            remaining -= size;
            mp3d->pcmOff += size;
            mp3d->pcmSize -= size;
            continue;
        }
        if (inLen > 0 && mp3d->mp3Size < sizeof(mp3d->mp3Buf)) {
            int8_t *inBuf = decoder->getBuf()->getData();
            int32_t size = std::min<int32_t>(sizeof(mp3d->mp3Buf) - mp3d->mp3Size, inLen);

            memmove(mp3d->mp3Buf, mp3d->mp3Buf + mp3d->mp3Off, mp3d->mp3Size);
            memcpy(mp3d->mp3Buf + mp3d->mp3Size, inBuf + inOff, size);

            inOff += size;
            inLen -= size;
            mp3d->mp3Off = 0;
            mp3d->mp3Size += size;
        }
        if (mp3d->mp3Size <= 0)
            break;
        int32_t n = mp3dec_decode_frame(mp3d, mp3d->mp3Buf + mp3d->mp3Off, mp3d->mp3Size, mp3d->pcmBuf, &info);
        if (n == 0 && info.frame_bytes == 0)
            break;
        mp3d->mp3Off += info.frame_bytes;
        mp3d->mp3Size -= info.frame_bytes;
        mp3d->pcmOff = 0;
        mp3d->pcmSize = (n * info.channels) << 1;
    }

    decoder->setOff(inOff);
    decoder->setLen(inLen);

    return len - remaining;
}

jvoid NativeMp3Decoder_Close(FNIEnv *env, jobject obj) {
    JMp3Decoder *decoder = (JMp3Decoder *)obj;
    Hook *hook = (Hook *)decoder->getHandle();
    if (hook != (Hook *)-1) {
        ((FExec *)env)->getFlint()->removeShutdownHook(hook);
        FlintAPI::System::free(hook->getHandle());
        decoder->setHandle(-1);
    }
}
