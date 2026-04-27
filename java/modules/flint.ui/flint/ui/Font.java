package flint.ui;

public final class Font {
    public static final Font DEFAULT = new Font("default", 12);

    private final String name;
    private final int size;

    public Font(String name, int size) {
        this.name = name;
        this.size = size;
    }

    public String name() {
        return name;
    }

    public int size() {
        return size;
    }
}
