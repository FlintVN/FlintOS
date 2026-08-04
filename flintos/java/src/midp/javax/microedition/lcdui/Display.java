package javax.microedition.lcdui;

import javax.microedition.midlet.MIDlet;

public class Display {
    static final Object LCDUILock = new Object();
    private static Display instance;
    private static int WIDTH;
    private static int HEIGHT;
    private static byte[] screenBuf;
    private static Graphics screenGfx;
    private Displayable current;

    static {
        WIDTH = flintos.device.Display.getWidth();
        HEIGHT = flintos.device.Display.getHeight();
        screenBuf = new byte[WIDTH * HEIGHT * 2];
        flint.drawing.Graphics fg = flint.drawing.Graphics.create(WIDTH, HEIGHT, screenBuf);
        screenGfx = new Graphics(fg);
        instance = new Display();
    }

    private Display() {

    }

    public static Display getDisplay(MIDlet m) {
        if(instance == null)
            instance = new Display();
        return instance;
    }

    public void setCurrent(Displayable d) {
        current = d;
        if(d instanceof Canvas) {
            ((Canvas)d).showNotify();
            requestPaint((Canvas)d);
        }
    }

    public void setCurrent(Alert alert, Displayable next) {
        /* No alert UI yet — go straight to the next displayable. */
        setCurrent(next);
    }

    public Displayable getCurrent() {
        return current;
    }

    public boolean isColor() {
        return true;
    }

    public int numColors() {
        return 65536;
    }

    public int numAlphaLevels() {
        return 1;
    }

    public void callSerially(Runnable r) {
        if(r != null)
            r.run();
    }

    public boolean flashBacklight(int ms) {
        return false;
    }

    public boolean vibrate(int ms) {
        return false;
    }

    public static Graphics gameGraphics() {
        return screenGfx;
    }

    public static void flush() {
        flintos.device.Display.write(0, 0, WIDTH, HEIGHT, screenBuf);
    }

    public static void flush(int x, int y, int width, int height) {
        flintos.device.Display.write(x, y, width, height, screenBuf);
    }

    int getDisplayWidth() {
        return WIDTH;
    }

    int getDisplayHeight() {
        return HEIGHT;
    }

    static int getPrimaryDisplayWidth() {
        return WIDTH;
    }

    static int getPrimaryDisplayHeight() {
        return HEIGHT;
    }

    static Displayable currentShown() {
        return instance == null ? null : instance.current;
    }

    static synchronized void requestPaint(Canvas c) {
        if(c == null)
            return;
        screenGfx.reset();
        c.paint(screenGfx);
        flintos.device.Display.write(0, 0, WIDTH, HEIGHT, screenBuf);
    }

    static synchronized void requestPaint(Canvas c, int x, int y, int width, int height) {
        if(c == null)
            return;
        screenGfx.reset();
        c.paint(screenGfx);
        flintos.device.Display.write(x, y, width, height, screenBuf);
    }
}
