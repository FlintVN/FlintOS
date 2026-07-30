package javax.microedition.lcdui;

public final class Font {
    public static final int FACE_SYSTEM = 0;
    public static final int FACE_MONOSPACE = 32;
    public static final int FACE_PROPORTIONAL = 64;
    public static final int STYLE_PLAIN = 0;
    public static final int STYLE_BOLD = 1;
    public static final int STYLE_ITALIC = 2;
    public static final int STYLE_UNDERLINED = 4;
    public static final int SIZE_SMALL = 8;
    public static final int SIZE_MEDIUM = 0;
    public static final int SIZE_LARGE = 16;
    public static final int FONT_STATIC_TEXT = 0;
    public static final int FONT_INPUT_TEXT = 1;

    private final int face;
    private final int style;
    private final int size;
    private final int height;
    private final int charW;

    private static Font DEFAULT_FONT;

    private Font(int face, int style, int size) {
        this.face = face; this.style = style; this.size = size;
        this.height = (size == SIZE_SMALL) ? 12 : (size == SIZE_LARGE) ? 20 : 16;
        this.charW  = this.height / 2;
    }

    public static synchronized Font getDefaultFont() {
        if(DEFAULT_FONT == null)
            DEFAULT_FONT = new Font(FACE_SYSTEM, STYLE_PLAIN, SIZE_MEDIUM);
        return DEFAULT_FONT;
    }

    public static Font getFont(int fontSpecifier) {
        return getDefaultFont();
    }

    public static Font getFont(int face, int style, int size) {
        return new Font(face, style, size);
    }

    public int getFace() {
        return face;
    }

    public int getStyle() {
        return style;
    }

    public int getSize() {
        return size;
    }

    public boolean isPlain() {
        return style == STYLE_PLAIN;
    }

    public boolean isBold() {
        return (style & STYLE_BOLD) != 0;
    }

    public boolean isItalic() {
        return (style & STYLE_ITALIC) != 0;
    }

    public boolean isUnderlined() {
        return (style & STYLE_UNDERLINED) != 0;
    }

    public int getHeight() {
        return height;
    }

    public int getBaselinePosition() {
        return height - 2;
    }

    public int charWidth(char c) {
        return charW;
    }

    public int charsWidth(char[] ch, int off, int len) {
        return len * charW;
    }

    public int stringWidth(String s) {
        return (s == null ? 0 : s.length()) * charW;
    }

    public int substringWidth(String s, int off, int len) {
        return len * charW;
    }
}
