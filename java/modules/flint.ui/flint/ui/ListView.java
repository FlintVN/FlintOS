package flint.ui;

public final class ListView implements Widget {
    private final String[] items;
    private final int x;
    private final int y;

    public ListView(String[] items, int x, int y) {
        this.items = items.clone();
        this.x = x;
        this.y = y;
    }

    @Override
    public void render() {
        for (int index = 0; index < items.length; index++) {
            Screen.drawText(items[index], x, y + index * 12);
        }
    }

    public String[] items() {
        return items.clone();
    }
}
