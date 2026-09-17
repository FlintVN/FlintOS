package flint.ui;

import flint.media.Display;
import flint.drawing.Color;
import flint.drawing.Graphics;
import flint.system.EventTypes;
import flint.system.NativeEvent;
import flint.system.NativeEventReceiver;

public abstract class FlintUI extends View {
    protected View content;

    private boolean doubleBuffer = true;
    private Display disp, disp1, disp2;

    private View actView;

    private KeyEvent keyEvent = new KeyEvent();
    private MotionEvent motionEvent = new MotionEvent();

    protected FlintUI() {
        this(240, 320);
    }

    protected FlintUI(int w, int h) {
        width = w;
        height = h;
        background = Theme.defaultTheme.backgroundColor();
    }

    @Override
    protected void onDraw(Graphics g) {
        if(content != null && content.isVisible(g))
            content.onDraw(g);
    }

    @Override
    protected View hitTest(int x, int y) {
        if(containsPoint(x, y)) {
            if(content != null) {
                View v = content.hitTest(x, y);
                if(v != null)
                    return v;
            }
            else
                return this;
        }
        return null;
    }

    protected void processEvent(NativeEvent event) {
        int eventType = event.getType();
        switch(eventType) {
            case EventTypes.KEY_EVENT: {
                keyEvent.action = event.getData(0);
                keyEvent.keyCode = event.getData(1);
                onKeyEvent(keyEvent);
                return;
            }
            case EventTypes.TOUCH_EVENT: {
                int action = event.getData(0);
                if(action == MotionEvent.ACTION_DOWN)
                    actView = hitTest(event.getData(1), event.getData(2));
                if(actView != null) {
                    motionEvent.action = action;
                    motionEvent.x = event.getData(1);
                    motionEvent.y = event.getData(2);
                    actView.onTouchEvent(motionEvent);
                }
                return;
            }
            case EventTypes.MONITOR_EVENT: {
                draw();
                return;
            }
        }
    }

    @Override
    public void setSize(int width, int height) {
        throw new UnsupportedOperationException();
    }

    @Override
    protected void updateLocation(int x, int y) {
        View v = content;
        if(v != null) {
            int xoff = x + switch(v.hAlignment.value) {
                case 0 -> v.marginLeft;
                case 1 -> (actualWidth - v.actualWidth) / 2 + v.marginLeft - v.marginRight;
                default -> actualWidth - v.marginRight - v.actualWidth;
            };

            int yoff = y + switch(v.vAlignment.value) {
                case 0 -> v.marginTop;
                case 1 -> (actualHeight - v.actualHeight) / 2 + v.marginTop - v.marginBottom;
                default -> actualHeight - v.marginBottom - v.actualHeight;
            };

            content.updateLocation(xoff, yoff);
        }
    }

    @Override
    protected void updateActualWidth(int availableW) {
        actualWidth = availableW;
        View v = content;
        if(v != null)
            v.updateActualWidth(availableW - v.marginLeft - v.marginRight);
    }

    @Override
    protected void updateActualHeight(int availableH) {
        actualHeight = availableH;
        View v = content;
        if(v != null)
            v.updateActualHeight(availableH - v.marginTop - v.marginBottom);
    }

    private void updateLayout() {
        updateActualWidth(width);
        updateActualHeight(height);
        updateLocation(0, 0);
    }

    public View getContent() {
        return content;
    }

    public void setContent(View v) {
        if(v != null) {
            if(v.parent != null)
                throw new IllegalStateException("The specified child already has a parent");
            v.parent = this;
        }
        if(content != null)
            content.parent = null;
        content = v;
    }

    @Override
    public void setBackground(Object bg) {
        if(bg == null)
            background = null;
        else if(bg instanceof Color)
            background = bg;
        else
            throw new IllegalArgumentException("background must be an instance of Color");
    }

    public void doubleBuffer(boolean enabled) {
        this.doubleBuffer = enabled;
    }

    public void show() {
        NativeEvent event = new NativeEvent();
        updateLayout();
        initGraphics();
        draw();
        while(true) {
            try {
                if(NativeEventReceiver.waitEvent(event))
                    processEvent(event);
            }
            catch(InterruptedException e) {
                
            }
        }
    }

    private void initGraphics() {
        disp1 = new Display(width, height);
        if(doubleBuffer)
            disp2 = new Display(width, height);
    }

    private void draw() {
        disp = (doubleBuffer && disp == disp1) ? disp2 : disp1;
        Color bg = background != null ? (Color)background : Theme.defaultTheme.backgroundColor();
        Graphics g = disp.createGraphics();

        g.setTransform(-x, -y);
        g.clear(bg);
        onDraw(g);

        disp.present();
    }
}
