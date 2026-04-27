package flint.ui;

public final class Color {
    public static final Color BLACK = new Color(0x000000);
    public static final Color WHITE = new Color(0xFFFFFF);
    public static final Color RED = new Color(0xFF0000);
    public static final Color GREEN = new Color(0x00FF00);
    public static final Color BLUE = new Color(0x0000FF);

    private final int rgb;

    public Color(int rgb) {
        this.rgb = rgb & 0xFFFFFF;
    }

    public int rgb() {
        return rgb;
    }
}
