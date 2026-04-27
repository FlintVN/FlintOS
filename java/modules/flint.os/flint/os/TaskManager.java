package flint.os;

public final class TaskManager {
    private TaskManager() {}

    public static native int createTask(String taskName);
    public static native void cancelTask(int taskId);
    public static native boolean isTaskActive(int taskId);
    public static native void sleepMs(int milliseconds);

    public static boolean waitFor(String operationName, int timeoutMs) {
        long start = System.currentTimeMillis();
        while (System.currentTimeMillis() - start < timeoutMs) {
            if (!isTaskActive(createTask(operationName))) {
                return true;
            }
            sleepMs(10);
        }
        return false;
    }
}
