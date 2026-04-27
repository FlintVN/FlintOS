package flint.ui;

public final class VirtualKeyboard implements Widget {
    @Override
    public void render() {
        Screen.drawText("Keyboard", 0, 0);
    }
}
