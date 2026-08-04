package javax.microedition.lcdui.game;

import javax.microedition.lcdui.Canvas;
import javax.microedition.lcdui.Display;
import javax.microedition.lcdui.Graphics;

public abstract class GameCanvas extends Canvas {
    public static final int UP_PRESSED = 1 << Canvas.UP;
    public static final int DOWN_PRESSED = 1 << Canvas.DOWN;
    public static final int LEFT_PRESSED = 1 << Canvas.LEFT;
    public static final int RIGHT_PRESSED = 1 << Canvas.RIGHT;
    public static final int FIRE_PRESSED = 1 << Canvas.FIRE;
    public static final int GAME_A_PRESSED = 1 << Canvas.GAME_A;
    public static final int GAME_B_PRESSED = 1 << Canvas.GAME_B;
    public static final int GAME_C_PRESSED = 1 << Canvas.GAME_C;
    public static final int GAME_D_PRESSED = 1 << Canvas.GAME_D;

    protected GameCanvas(boolean suppressKeyEvents) {

    }

    protected Graphics getGraphics() {
        return Display.gameGraphics();
    }

    public int getKeyStates() {
        // TODO
        return 0;
    }

    public void paint(Graphics g) {

    }

    public void flushGraphics(int x, int y, int width, int height) {
        Display.flush(x, y, width, height);
    }

    public void flushGraphics() {
        Display.flush();
    }
}
