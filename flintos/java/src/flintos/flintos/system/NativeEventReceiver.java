package flintos.system;

public class NativeEventReceiver {
    private NativeEventReceiver() {

    }

    public static native boolean waitEvent(NativeEvent event) throws InterruptedException;

    public static native boolean waitEvent(NativeEvent event, long millis) throws InterruptedException;
}
