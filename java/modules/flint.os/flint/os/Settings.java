package flint.os;

public final class Settings {
    private Settings() {}

    public static native String getString(String key, String fallback);
}
