package javax.microedition.lcdui;

import javax.microedition.midlet.MIDlet;
import com.sun.midp.events.EventQueue;
import com.sun.midp.lcdui.RepaintEventProducer;

public class Display {
    static final Object LCDUILock = new Object();
    static RepaintEventProducer repaintEventProducer;
    private static Display instance;
    private static int WIDTH;
    private static int HEIGHT;
    private static flintos.device.Display flintDisplay;
    private static Graphics screenGfx;
    private Displayable current;

    static {
        WIDTH = flintos.device.Display.getPrimaryWidth();
        HEIGHT = flintos.device.Display.getPrimaryHeight();
        flintDisplay = new flintos.device.Display(WIDTH, HEIGHT);
        screenGfx = new Graphics(flint.drawing.Graphics.create(WIDTH, HEIGHT, flintDisplay.getBuffer()));
        instance = new Display();
        repaintEventProducer = new RepaintEventProducer(EventQueue.getEventQueue());
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
        if(d instanceof Canvas canvas) {
            canvas.showNotify();
            repaintEventProducer.scheduleRepaint(0, 0, WIDTH, HEIGHT, canvas);
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
        flintDisplay.present();
    }

    public static void flush(int x, int y, int width, int height) {
        flintDisplay.present(x, y, width, height);
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

    public static void handleRepaintEvent(int x1, int y1, int x2, int y2, Canvas target) {
        if(target == null)
            return;
        screenGfx.reset();
        target.paint(screenGfx);
        flintDisplay.present(x1, y1, x2 - x1, y2 - y1);
    }
}
