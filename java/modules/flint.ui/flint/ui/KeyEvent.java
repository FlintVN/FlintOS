package flint.ui;

public final class KeyEvent {
    private final int keyCode;
    private final boolean pressed;

    public KeyEvent(int keyCode, boolean pressed) {
        this.keyCode = keyCode;
        this.pressed = pressed;
    }

    public int keyCode() { return keyCode; }
    public boolean pressed() { return pressed; }
}
