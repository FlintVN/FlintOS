package javax.microedition.lcdui.game;

import javax.microedition.lcdui.Canvas;
import javax.microedition.lcdui.Display;
import javax.microedition.lcdui.Graphics;

/* MIDP GameCanvas: active rendering via getGraphics()/flushGraphics() on the shared screen
 * framebuffer, plus a polled key-state bitmask. Subclasses that override keyPressed (like
 * Diamond Rush's `e`) get events directly; getKeyStates() also works for polling games. */
public abstract class GameCanvas extends Canvas {
    public static final int UP_PRESSED = 1 << 1, LEFT_PRESSED = 1 << 2, RIGHT_PRESSED = 1 << 5,
        DOWN_PRESSED = 1 << 6, FIRE_PRESSED = 1 << 8,
        GAME_A_PRESSED = 1 << 9, GAME_B_PRESSED = 1 << 10, GAME_C_PRESSED = 1 << 11, GAME_D_PRESSED = 1 << 12;

    private volatile int keyStates = 0;
    private int touchPrev = -1;

    protected GameCanvas(boolean suppressKeyEvents) {}

    protected Graphics getGraphics() { return Display.gameGraphics(); }

    /* Deliver on-screen-keypad input HERE, on the game's own thread, every frame the game
     * flushes. Doing it on a separate thread needs that thread to out-prioritise the game's
     * flat-out render loop, which instead starved/stalled the game. board.Touch.poll() returns
     * the ASCII key under the finger (or -1); we turn changes into keyPressed/keyReleased on
     * this canvas (virtual dispatch -> the game's own overrides). */
    private void pumpTouch() {
        int cur = flintos.midp.TouchBridge.poll();
        if(cur == touchPrev) return;
        if(touchPrev != -1) keyReleased(touchPrev);
        if(cur != -1) keyPressed(cur);
        touchPrev = cur;
    }

    public void flushGraphics() { pumpTouch(); Display.flush(); }
    public void flushGraphics(int x, int y, int width, int height) { pumpTouch(); Display.flush(); }
    public int getKeyStates() { return keyStates; }

    /* GameCanvas supplies a concrete paint(), so games that render actively via
     * getGraphics()/flushGraphics() (like Diamond Rush's `i`) need not override it.
     * The passive repaint() path (Display.requestPaint) calls this; for an active
     * renderer it's a no-op — the run() loop owns the framebuffer. */
    public void paint(Graphics g) {}

    /* Keep the polled bitmask in sync with delivered key events (injected or, later, touch). */
    protected void keyPressed(int keyCode) { keyStates |= toState(keyCode); }
    protected void keyReleased(int keyCode) { keyStates &= ~toState(keyCode); }

    private int toState(int keyCode) {
        switch(getGameAction(keyCode)) {
            case UP:     return UP_PRESSED;
            case DOWN:   return DOWN_PRESSED;
            case LEFT:   return LEFT_PRESSED;
            case RIGHT:  return RIGHT_PRESSED;
            case FIRE:   return FIRE_PRESSED;
            case GAME_A: return GAME_A_PRESSED;
            case GAME_B: return GAME_B_PRESSED;
            case GAME_C: return GAME_C_PRESSED;
            case GAME_D: return GAME_D_PRESSED;
            default:     return 0;
        }
    }
}
