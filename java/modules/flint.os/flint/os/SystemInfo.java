package flint.os;

public final class SystemInfo {
    private SystemInfo() {}

    public static native String osVersion();
    public static native String boardName();
}
