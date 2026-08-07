package javax.microedition.lcdui;

public abstract class Canvas extends Displayable {
    public static final int UP = 1;
    public static final int DOWN = 6;
    public static final int LEFT = 2;
    public static final int RIGHT = 5;
    public static final int FIRE = 8;
    public static final int GAME_A = 9;
    public static final int GAME_B = 10;
    public static final int GAME_C = 11;
    public static final int GAME_D = 12;
    public static final int KEY_NUM0 = 48;
    public static final int KEY_NUM1 = 49;
    public static final int KEY_NUM2 = 50;
    public static final int KEY_NUM3 = 51;
    public static final int KEY_NUM4 = 52;
    public static final int KEY_NUM5 = 53;
    public static final int KEY_NUM6 = 54;
    public static final int KEY_NUM7 = 55;
    public static final int KEY_NUM8 = 56;
    public static final int KEY_NUM9 = 57;
    public static final int KEY_STAR = 42;
    public static final int KEY_POUND = 35;

    protected Canvas() {

    }

    public boolean isDoubleBuffered() {
        return false;
    }

    public boolean hasPointerEvents() {
        return false;
    }

    public boolean hasPointerMotionEvents() {
        return false;
    }

    public boolean hasRepeatEvents() {
        return false;
    }

    public int getKeyCode(int gameAction) {
        int n = KeyConverter.getKeyCode(gameAction);

        if(n == 0)
            throw new IllegalArgumentException();

        return n;
    }

    public String getKeyName(int keyCode) {
        String s = KeyConverter.getKeyName(keyCode);

        if(s == null)
            throw new IllegalArgumentException();

        return s;
    }

    public int getGameAction(int keyCode) {
        int n = KeyConverter.getGameAction(keyCode);

        if(n == -1)
            throw new IllegalArgumentException();

        return n;
    }

    public void setFullScreenMode(boolean mode) {
        isInFullScreenMode = mode;
    }

    protected void keyPressed(int keyCode) {

    }

    protected void keyRepeated(int keyCode) {

    }

    protected void keyReleased(int keyCode) {

    }

    protected void pointerPressed(int x, int y) {

    }

    protected void pointerReleased(int x, int y) {

    }

    protected void pointerDragged(int x, int y) {

    }

    public final void repaint(int x, int y, int width, int height) {
        Display.repaintEventProducer.scheduleRepaint(x, y, width, height, this);
    }

    public final void repaint() {
        repaint(0, 0, Display.getPrimaryDisplayWidth(), Display.getPrimaryDisplayHeight());
    }

    public final void serviceRepaints() {
        Display.repaintEventProducer.serviceRepaints();
    }

    protected void showNotify() {

    }

    protected void hideNotify() {

    }

    protected abstract void paint(Graphics g);

    protected void sizeChanged(int w, int h) {

    }
}
