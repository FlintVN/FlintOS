package flintos.device;

public class Display {
    private int width;
    private int height;
    private byte[] buffer;

    public Display(int width, int height) {
        if(width < 0 || height < 0)
            throw new IllegalArgumentException("width and height must be non-negative: width=" + width + ", height=" + height);
        this.width = width;
        this.height = height;
        this.buffer = new byte[width * height * 2];
    }

    public static native int getPrimaryWidth();

    public static native int getPrimaryHeight();

    public static native boolean isForeground();

    public static native void setBrightness(int value);

    public int getWidth() {
        return width;
    }

    public int getHeight() {
        return height;
    }

    public byte[] getBuffer() {
        return buffer;
    }

    public native void present();

    public native void present(int x, int y, int w, int h);
}
