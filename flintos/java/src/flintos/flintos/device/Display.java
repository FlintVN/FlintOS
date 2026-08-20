package flintos.device;

public class Display {
    private Display() {

    }

    public static native int getWidth();

    public static native int getHeight();

    public static native boolean isForeground();

    public static native void write(int x, int y, int w, int h, byte[] data);

    public static native void setBrightness(int value);
}
