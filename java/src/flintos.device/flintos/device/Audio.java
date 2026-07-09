package flintos.device;

public class Audio implements AutoCloseable {
    private int pos = -1;
    private int length;

    public static native int getSampleRate();

    public static native int getChannels();

    public static native int getFrameSize();

    public static native int getVolumn();

    public static native void setVolumn(int value);

    public Audio() {

    }

    public native void open();

    public native void write(byte[] b);

    public native void close();
}
