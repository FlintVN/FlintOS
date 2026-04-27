package flint.io;

public final class Pin {
    private Pin() {}

    public static native void write(int pin, boolean value);
    public static native boolean read(int pin);
}
