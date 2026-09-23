package flint.ui;

import flint.media.Display;
import flint.drawing.Color;
import flint.drawing.Graphics;
import flint.system.EventTypes;
import flint.system.NativeEvent;
import flint.system.NativeEventReceiver;

public abstract class FlintUI extends View {
    private static FlintUI currentUI;

    private Thread mThread;

    protected View content;

    private boolean doubleBuffer = true;
    private Display disp, disp1, disp2;

    private boolean invLayout = true;
    private int invX, invY, invW, invH;

    private View actView;

    private KeyEvent keyEvent = new KeyEvent();
    private MotionEvent motionEvent = new MotionEvent();

    private TaskQueue taskQueue = new TaskQueue();

    protected FlintUI() {
        this(240, 320);
    }

    protected FlintUI(int w, int h) {
        mThread = Thread.currentThread();
        width = w;
        height = h;
        invX = 0;
        invY = 0;
        invW = w;
        invH = h;
        background = Theme.defaultTheme.backgroundColor();
    }

    public static void checkThread() {
        if(currentUI != null && currentUI.mThread != Thread.currentThread())
            throw new CalledFromWrongThreadException("Only the original thread that created a view hierarchy can touch its views");
    }

    static final void setInvalidate(int x, int y, int w, int h, boolean layout) {
        FlintUI ui = currentUI;
        if(ui == null)
            return;
        if(ui.invH == 0) {
            ui.invX = x;
            ui.invY = y;
            ui.invW = w;
            ui.invH = h;
        }
        else {
            int x2 = x + w;
            int y2 = y + h;
            if(ui.invX > x) ui.invX = x;
            if(ui.invY > y) ui.invY = y;
            if((ui.invX + ui.invW) < x2) ui.invW = x2 - ui.invX;
            if((ui.invY + ui.invH) < y2) ui.invH = y2 - ui.invY;
        }
        if(layout)
            ui.invLayout = layout;
    }

    static final void setInvalidateAll() {
        FlintUI ui = currentUI;
        if(ui == null)
            return;
        ui.invX = 0;
        ui.invY = 0;
        ui.invW = ui.actualWidth;
        ui.invH = ui.actualHeight;
        ui.invLayout = true;
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
                    actView.dispatchTouchEvent(motionEvent);
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
        currentUI = this;
        NativeEvent event = new NativeEvent();
        initGraphics();
        draw();
        while(true) {
            try {
                if(NativeEventReceiver.waitEvent(event))
                    processEvent(event);
                taskQueue.runAll();
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
        int w = invW;
        int h = invH;
        if(invLayout || (w > 0 && h > 0)) {
            int x = invX;
            int y = invY;
            invW = 0;
            invH = 0;
            if(invLayout) {
                invLayout = false;
                updateLayout();
            }

            disp = (doubleBuffer && disp == disp1) ? disp2 : disp1;
            Color bg = background != null ? (Color)background : Theme.defaultTheme.backgroundColor();
            Graphics g = disp.createGraphics();
            g.setClip(x, y, w, h);
            g.clear(bg);
            onDraw(g);
            disp.present(x, y, w, h);
        }
    }

    public final void runOnUiThread(Runnable task) {
        if(mThread == Thread.currentThread())
            task.run();
        else {
            taskQueue.post(task);
            NativeEventReceiver.notifyEvent();
        }
    }

    private class TaskQueue {
        Object queueLock;

        Task head;
        Task last;

        public TaskQueue() {
            queueLock = new Object();
        }

        public void post(Runnable task) {
            Task t = new Task(task);
            synchronized(queueLock) {
                if(last != null)
                    last.next = t;
                else
                    head = t;
                last = t;
            }
        }

        public void runAll() {
            Task task;
            if(head == null)
                return;
            synchronized(queueLock) {
                task = head;
                head = null;
                last = null;
            }

            while(task != null) {
                task.run();
                task = task.next;
            }
        }

        private class Task {
            Runnable task;
            Task next;

            public Task(Runnable task) {
                if(task == null)
                    throw new NullPointerException();
                this.task = task;
            }

            public void run() {
                task.run();
            }
        }
    }
}
