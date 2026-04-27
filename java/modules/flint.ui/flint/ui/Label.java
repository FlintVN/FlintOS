package flint.ui;

public final class Label implements Widget {
    private final String text;
    private final int x;
    private final int y;

    public Label(String text, int x, int y) {
        this.text = text;
        this.x = x;
        this.y = y;
    }

    @Override
    public void render() {
        Screen.drawText(text, x, y);
    }

    public String text() {
        return text;
    }
}
