package flint.app;

public final class AppManager {
    private AppManager() {}

    public static native AppInfo[] installedApps();

    public static native boolean launch(String appId);
}
