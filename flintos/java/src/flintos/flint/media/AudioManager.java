package flint.media;

public class AudioManager {
    private AudioManager() {

    }

    public static native int getVolume();

    public static native void setVolume(int value);
}
