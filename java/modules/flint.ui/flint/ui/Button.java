package flint.ui;

public final class Button implements Widget {
    private final String label;
    private final int x;
    private final int y;

    public Button(String label, int x, int y) {
        this.label = label;
        this.x = x;
        this.y = y;
    }

    @Override
    public void render() {
        Screen.drawText("[ " + label + " ]", x, y);
    }

    public String label() {
        return label;
    }
}
