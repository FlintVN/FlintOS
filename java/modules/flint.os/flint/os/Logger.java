package flint.os;

public final class Logger {
    private Logger() {}

    public static native void info(String message);
    public static native void warn(String message);
    public static native void error(String message);
}
