package flint.ui;

public final class TextInput implements Widget {
    private String value;

    public TextInput(String value) {
        this.value = value;
    }

    @Override
    public void render() {
        Screen.drawText(value, 0, 0);
    }

    public String value() {
        return value;
    }

    public void setValue(String value) {
        this.value = value;
    }
}
