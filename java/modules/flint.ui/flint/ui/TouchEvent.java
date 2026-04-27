package flint.ui;

public final class TouchEvent {
    private final int x;
    private final int y;
    private final boolean pressed;

    public TouchEvent(int x, int y, boolean pressed) {
        this.x = x;
        this.y = y;
        this.pressed = pressed;
    }

    public int x() { return x; }
    public int y() { return y; }
    public boolean pressed() { return pressed; }
}
