package flintos.midp;

/**
 * Native bridge to the board's on-screen touch keypad.
 *
 * <p>Replaces the legacy {@code board.Touch} class.  The keypad is drawn
 * natively in the bottom strip of the panel; {@link #poll()} reports the
 * key currently under the finger.</p>
 *
 * <p>Native methods are registered as
 * {@code NATIVE_CLASS("flintos/midp/TouchBridge", ...)}.</p>
 */
public final class TouchBridge {
    /** Bring up the I2C touch controller and draw the on-screen keypad. */
    public static native void init();

    /**
     * ASCII code of the keypad button currently under the finger
     * ({@code '0'..'9', '*', '#'}), or -1 if nothing is pressed.
     * Non-blocking.
     */
    public static native int poll();

    /**
     * Raise the calling thread's scheduler priority above the game thread
     * so the input poll loop is never starved by the game's render loop.
     * Call once from the input thread.
     */
    public static native void boost();

    private TouchBridge() {}
}
