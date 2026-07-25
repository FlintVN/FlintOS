package flintos.midp;

/**
 * Native bridge to the board LCD panel.
 *
 * <p>Replaces the legacy {@code board.Lcd} class with a package that lives
 * in the FlintOS device layer.  MIDP applications never touch this class
 * directly — they use {@code javax.microedition.lcdui.Display} which
 * delegates here.</p>
 *
 * <p>Native methods are registered as
 * {@code NATIVE_CLASS("flintos/midp/DisplayBridge", ...)}.</p>
 */
public final class DisplayBridge {
    /** Initialize the LCD panel (physical dimensions). */
    public static native void init();

    /** Initialize the LCD with logical dimensions for landscape games. */
    public static native void init(int width, int height, String mode);

    /** Logical panel width in pixels. */
    public static native int width();

    /** Logical panel height in pixels. */
    public static native int height();

    /**
     * Push a full big-endian RGB565 framebuffer to the panel.
     * Array length must be {@code width() * height() * 2}.
     */
    public static native void present(byte[] fb);

    /**
     * Next typed console byte (from the serial monitor) as a game key,
     * or -1 if none.  Non-blocking; bytes are queued by the firmware's
     * serial RX path.
     */
    public static native int readKey();

    /** Called when Display singleton is created. */
    public static native void onDisplayCreated();

    /** Called when a Canvas is shown (set as current displayable). */
    public static native void onCanvasShown(int width, int height);

    /** Called when Graphics is reset before each paint. */
    public static native void onGraphicsReset();

    private DisplayBridge() {}
}
