package flintos.device;

public class AudioTrack implements AutoCloseable {
    private int pos = -1;

    private int sampleRate;
    private int audioFormat;
    private int channels;

    private byte[] buff;
    private int bufPos;

    private int srcPos;
    private int srcAcc;

    private static native int getFrameSize();

    public static native int getVolumn();

    public static native void setVolumn(int value);

    public AudioTrack(int sampleRate, int audioFormat, int channels) {
        this.sampleRate = sampleRate;
        this.audioFormat = audioFormat;
        this.channels = channels;
        this.buff = new byte[getFrameSize()];
    }

    public int getSampleRate() {
        return sampleRate;
    }

    public int getChannels() {
        return channels;
    }

    public void open() {
        if(pos >= 0)
            throw new IllegalStateException("AudioTrack has been opened");
        pos = open0();
    }

    private native int open0();

    public native void write(byte[] b);

    public native void write(byte[] b, int off, int len);

    public void close() {
        this.pos = -1;
        this.bufPos = 0;
        this.srcPos = 0;
        this.srcAcc = 0;
    }
}
