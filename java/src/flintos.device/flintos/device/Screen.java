package flintos.device;

public class Screen {
    private Screen() {

    }

    public static native void write(int x, int y, int w, int h, byte[] data);

    public static native void setBrightness(int value);
}
