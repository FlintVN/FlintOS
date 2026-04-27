package flint.ui;

public final class Dialog implements Widget {
    private final String title;
    private final String message;

    public Dialog(String title, String message) {
        this.title = title;
        this.message = message;
    }

    @Override
    public void render() {
        Screen.clear();
        Screen.drawText(title, 0, 0);
        Screen.drawText(message, 0, 16);
    }
}
