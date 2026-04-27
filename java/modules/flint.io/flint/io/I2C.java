package flint.io;

public final class I2C {
    private I2C() {}

    public static native boolean isAvailable(int bus);
}
