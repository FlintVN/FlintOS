package flint.os;

public final class DownloadManager {
    private DownloadManager() {}

    public static native boolean downloadUrl(String url, String outputPath);
    public static native String downloadText(String url, int timeoutMs);
}
