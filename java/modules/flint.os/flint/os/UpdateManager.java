package flint.os;

public final class UpdateManager {
    private UpdateManager() {}

    public static native boolean hasPendingValidUpdate();
    public static native boolean stageUpdate(String metadataUrl, String expectedChecksum);
    public static native void markBootHealthy(boolean healthy);
}
