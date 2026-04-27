package flint.os;

public final class Storage {
    private Storage() {}

    public static native boolean exists(String path);
    public static native String rootPath();
}
