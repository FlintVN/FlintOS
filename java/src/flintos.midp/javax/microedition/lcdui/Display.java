package javax.microedition.lcdui;

import flintos.midp.DisplayBridge;

/* MIDP Display + the (synchronous) paint pump. One screen-sized RGB565 buffer is shared
 * by all paints; repaint() renders the current Canvas into it and pushes it to the panel
 * via Lcd.present(). No render thread for v1 — the game's own loop thread calls repaint(). */
public class Display {
    private static Display instance;

    private static int sw, sh;
    private static byte[] screenBuf;
    private static Graphics screenGfx;

    private Displayable current;

    private Display() {}

    /** Launcher boot: bring up the LCD and the screen framebuffer. (Not MIDP API.) */
    public static synchronized void initScreen() {
        if(screenBuf != null) return;
        DisplayBridge.init();
        sw = DisplayBridge.width();
        sh = DisplayBridge.height();
        screenBuf = new byte[sw * sh * 2];
        flint.drawing.Graphics fg = flint.drawing.Graphics.create(sw, sh, screenBuf);
        screenGfx = new Graphics(fg, sw, sh);
        if(instance == null) instance = new Display();
    }

    public static Display getDisplay(javax.microedition.midlet.MIDlet m) {
        if(instance == null) initScreen();
        return instance;
    }

    public void setCurrent(Displayable d) {
        current = d;
        if(d instanceof Canvas) {
            ((Canvas) d).showNotify();
            requestPaint((Canvas) d);
        }
    }
    public void setCurrent(Alert alert, Displayable next) {
        /* No alert UI yet — go straight to the next displayable. */
        setCurrent(next);
    }
    public Displayable getCurrent() { return current; }

    public boolean isColor() { return true; }
    public int numColors() { return 65536; }
    public int numAlphaLevels() { return 1; }
    public void callSerially(Runnable r) { if(r != null) r.run(); }
    public boolean flashBacklight(int ms) { return false; }
    public boolean vibrate(int ms) { return false; }

    /* ---- MIDP 2.0 image sizing ---- */
    public static final int LIST = 1, CHOICE_GROUP = 2, ALERT = 3;
    public int getBestImageWidth(int imageType) {
        switch(imageType) {
            case LIST:   return sw;
            case ALERT:  return sw * 2 / 3;
            default:     return 0;
        }
    }
    public int getBestImageHeight(int imageType) {
        switch(imageType) {
            case LIST:   return sh / 5;
            case ALERT:  return sh / 3;
            default:     return 0;
        }
    }
    public void setCurrentItem(Item item) {
        if(item != null && item.getOwner() instanceof Form) {
            setCurrent((Form) item.getOwner());
        }
    }

    /* ---- GameCanvas hooks: shared screen framebuffer + present ---- */
    private static long flushCount = 0;
    public static Graphics gameGraphics() { return screenGfx; }
    public static void flush() {
        if(screenBuf == null) return;
        DisplayBridge.present(screenBuf);
    }

    /* ---- internal pump ---- */
    static int screenWidth()  { return sw; }
    static int screenHeight() { return sh; }
    static Displayable currentShown() { return instance == null ? null : instance.current; }

    static synchronized void requestPaint(Canvas c) {
        if(screenGfx == null || c == null) return;
        screenGfx.reset();
        c.paint(screenGfx);
        DisplayBridge.present(screenBuf);
    }
}
