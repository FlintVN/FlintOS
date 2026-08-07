package com.sun.midp.lcdui;

import com.sun.midp.events.Event;
import com.sun.midp.events.EventTypes;
import javax.microedition.lcdui.Canvas;

class RepaintEvent extends Event {
    int paintX1;
    int paintY1;
    int paintX2;
    int paintY2;
    int perUseID;
    Canvas paintTarget;

    private RepaintEvent(int type) {
        super(type);
    }

    static RepaintEvent createRepaintEvent(int x, int y, int w, int h, Canvas target) {
        RepaintEvent e = new RepaintEvent(EventTypes.REPAINT_EVENT);
        e.setRepaintFields(x, y, w, h, target);
        return e;
    }

    void setRepaintFields(int x, int y, int w, int h, Canvas target) {
        w += x;
        h += y;

        if(x < 0)
            x = 0;

        if(y < 0)
            y = 0;

        paintX1 = x;
        paintY1 = y;
        paintX2 = w;
        paintY2 = h;
        paintTarget = target;
    }
}
