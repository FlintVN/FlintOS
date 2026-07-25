package javax.microedition.media;

import flintos.device.Audio;
import java.io.IOException;
import java.io.InputStream;

/** PCM WAV player backed by the FlintOS audio service. */
final class WavePlayer extends AbstractPlayer {
    private static final int PCM_FORMAT = 1;

    private final String type;
    private final byte[] source;

    private int channels;
    private int sampleRate;
    private int bitsPerSample;
    private int dataOffset;
    private int dataLength;
    private int sourceFrameSize;
    private int sourceFramePosition;
    private long duration;

    WavePlayer(InputStream stream, String type) throws IOException, MediaException {
        this.type = type;
        try(InputStream input = stream) {
            source = input.readAllBytes();
        }
        parseHeader();
    }

    public long getDuration() {
        return duration;
    }

    protected void playOnce() throws Exception {
        int outputRate = Audio.getSampleRate();
        int outputFrameSize = Audio.getFrameSize();
        if(outputFrameSize < 2)
            throw new MediaException("Invalid audio frame size");
        byte[] output = new byte[outputFrameSize];
        long startedAt = System.currentTimeMillis();
        long startedMediaTime = getMediaTime();

        try(Audio audio = new Audio()) {
            audio.open();
            while(!isStopRequested() && sourceFramePosition < sourceFrames()) {
                int outputSamples = output.length / 2;
                int produced = 0;
                for(int i = 0; i < outputSamples; i++) {
                    int sourceFrame = sourceFramePosition
                            + (int)((long)i * sampleRate / outputRate);
                    if(sourceFrame >= sourceFrames())
                        break;
                    int sample = readMonoSample(sourceFrame);
                    sample = getVolumeControl().scale(sample);
                    output[produced++] = (byte)sample;
                    output[produced++] = (byte)(sample >>> 8);
                }
                if(produced == 0)
                    break;
                while(produced < output.length)
                    output[produced++] = 0;
                audio.write(output);

                int advanced = Math.max(1,
                        (int)((long)outputSamples * sampleRate / outputRate));
                sourceFramePosition += advanced;
                long mediaTime = (long)sourceFramePosition * 1000000L / sampleRate;
                setPlaybackTime(Math.min(duration, mediaTime));

                long targetElapsed = (getMediaTime() - startedMediaTime) / 1000L;
                long delay = targetElapsed - (System.currentTimeMillis() - startedAt);
                if(delay > 0)
                    Thread.sleep(delay);
            }
        }
    }

    protected long seek(long mediaTime) {
        long bounded = Math.min(duration, mediaTime);
        sourceFramePosition = (int)(bounded * sampleRate / 1000000L);
        return (long)sourceFramePosition * 1000000L / sampleRate;
    }

    protected String contentType() {
        return type;
    }

    private void parseHeader() throws MediaException {
        if(source.length < 44 || readInt(0) != 0x46464952 || readInt(8) != 0x45564157)
            throw new MediaException("Invalid WAV header");

        int format = -1;
        int offset = 12;
        while(offset <= source.length - 8) {
            int chunk = readInt(offset);
            int length = readLittleInt(offset + 4);
            int payload = offset + 8;
            if(length < 0 || payload > source.length - length)
                throw new MediaException("Invalid WAV chunk");
            if(chunk == 0x20746D66) {
                if(length < 16)
                    throw new MediaException("Invalid WAV format chunk");
                format = readLittleShort(payload);
                channels = readLittleShort(payload + 2);
                sampleRate = readLittleInt(payload + 4);
                bitsPerSample = readLittleShort(payload + 14);
            }
            else if(chunk == 0x61746164) {
                dataOffset = payload;
                dataLength = length;
            }
            offset = payload + length + (length & 1);
        }

        if(format != PCM_FORMAT)
            throw new MediaException("Only PCM WAV is supported");
        if(channels != 1 && channels != 2)
            throw new MediaException("Unsupported channel count: " + channels);
        if(bitsPerSample != 8 && bitsPerSample != 16)
            throw new MediaException("Unsupported PCM depth: " + bitsPerSample);
        if(sampleRate <= 0 || dataLength <= 0)
            throw new MediaException("Incomplete WAV file");

        sourceFrameSize = channels * bitsPerSample / 8;
        duration = (long)sourceFrames() * 1000000L / sampleRate;
    }

    private int sourceFrames() {
        return dataLength / sourceFrameSize;
    }

    private int readMonoSample(int frame) {
        int offset = dataOffset + frame * sourceFrameSize;
        int left = readSample(offset);
        if(channels == 1)
            return left;
        int right = readSample(offset + bitsPerSample / 8);
        return (left + right) / 2;
    }

    private int readSample(int offset) {
        if(bitsPerSample == 8)
            return ((source[offset] & 0xFF) - 128) << 8;
        return (short)((source[offset] & 0xFF) | (source[offset + 1] << 8));
    }

    private int readInt(int offset) {
        return (source[offset] & 0xFF) << 24
                | (source[offset + 1] & 0xFF) << 16
                | (source[offset + 2] & 0xFF) << 8
                | source[offset + 3] & 0xFF;
    }

    private int readLittleInt(int offset) {
        return source[offset] & 0xFF
                | (source[offset + 1] & 0xFF) << 8
                | (source[offset + 2] & 0xFF) << 16
                | (source[offset + 3] & 0xFF) << 24;
    }

    private int readLittleShort(int offset) {
        return source[offset] & 0xFF | (source[offset + 1] & 0xFF) << 8;
    }
}
