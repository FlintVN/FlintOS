package flintos.midp;

import javax.microedition.midlet.MIDlet;

/**
 * FlintOS MIDlet lifecycle manager.
 */
public final class MIDletLifecycle {
    public static final int PAUSED = 0;
    public static final int ACTIVE = 1;
    public static final int DESTROYED = 2;

    private static MIDlet currentMIDlet;
    private static int state = DESTROYED;

    private MIDletLifecycle() {}

    public static void init() {
    }

    public static synchronized void attach(MIDlet midlet) {
        if(midlet == null) throw new NullPointerException("midlet");
        currentMIDlet = midlet;
        state = PAUSED;
    }

    public static synchronized void start(MIDlet midlet) {
        if(midlet == null) throw new NullPointerException("midlet");
        if(state == DESTROYED) throw new IllegalStateException("MIDlet destroyed");
        if(state == ACTIVE) return;
        state = ACTIVE;
        try {
            midlet.startApp0();
        } catch(Throwable t) {
            state = DESTROYED;
            throw new RuntimeException(t);
        }
    }

    public static synchronized void pause(MIDlet midlet) {
        if(midlet == null || midlet != currentMIDlet) return;
        if(state != ACTIVE) return;
        midlet.pauseApp0();
        state = PAUSED;
    }

    public static synchronized void destroy(MIDlet midlet, boolean unconditional) {
        if(midlet == null || midlet != currentMIDlet) return;
        if(state == DESTROYED) return;
        try {
            midlet.destroyApp0(unconditional);
        } catch(Throwable t) {
            if(!unconditional) throw new RuntimeException(t);
        }
        state = DESTROYED;
    }

    public static synchronized int getState() { return state; }
}
