package flint.ui;

public final class Screen {
    private Screen() {}

    public static native void clear();
    public static native void drawText(String text, int x, int y);
}
