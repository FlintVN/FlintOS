package flint.app;

public final class AppManager {
    private AppManager() {}

    public static native AppInfo[] installedApps();

    public static native boolean launch(String appId);

    public static boolean startActivity(Intent intent) {
        if (intent.targetAppId() != null) {
            return launch(intent.targetAppId());
        }
        AppInfo[] handlers = queryIntentActivities(intent);
        return handlers.length == 1 && launch(handlers[0].id());
    }

    public static boolean startActivityForResult(Intent intent) {
        return startActivity(intent);
    }

    public static native void finishActivity();

    public static native void registerIntentHandler(String action, String appId);

    public static native AppInfo[] queryIntentActivities(Intent intent);
}
