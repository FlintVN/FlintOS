package flint.io;

public final class SPI {
    private SPI() {}

    public static native boolean isAvailable(int bus);
}
