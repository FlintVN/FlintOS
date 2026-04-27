package flint.os;

public final class Power {
    private Power() {}

    public static native int batteryPercent();
    public static native boolean isCharging();
}
