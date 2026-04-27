package flint.net;

public final class WiFi {
    private WiFi() {}

    public static native boolean isEnabled();
    public static native boolean connect(String ssid);
}
