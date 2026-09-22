package flint.ui;

import flint.drawing.Size;
import flint.drawing.Color;
import flint.drawing.Image;
import flint.drawing.Graphics;

public abstract class View {
    public static final int MATCH_PARENT = -1;
    public static final int WRAP_CONTENT = -2;

    private static final int SCROLL_LIMIT = 10;

    protected View parent;

    protected boolean visible = true;

    protected int width;
    protected int height;

    protected int x;
    protected int y;

    protected int actualWidth;
    protected int actualHeight;

    protected int marginLeft;
    protected int marginTop;
    protected int marginRight;
    protected int marginBottom;

    protected Object background;

    protected HorizontalAlignment hAlignment = HorizontalAlignment.LEFT;
    protected VerticalAlignment vAlignment = VerticalAlignment.TOP;

    private OnTouchListener onTouchListener;
    private OnClickListener onClickListener;

    protected boolean isPressing;
    protected int startX, startY;

    public View() {

    }

    public void invalidateVisual() {
        FlintUI.setInvalidateVisual(x, y, actualWidth, actualHeight);
    }

    public void invalidateLayout() {
        FlintUI.setInvalidateLayout();
    }

    public void invalidateInternalLayout() {
        FlintUI.setInvalidateLayout(x, y, actualWidth, actualHeight);
    }

    protected abstract void onDraw(Graphics g);

    protected boolean containsPoint(int x, int y) {
        return this.x <= x && x < (this.x + actualWidth) && this.y <= y && y < (this.y + actualHeight);
    }

    protected View hitTest(int x, int y) {
        return containsPoint(x, y) ? this : null;
    }

    protected boolean manipulationMode() {
        return false;
    }

    protected void onKeyEvent(KeyEvent event) {

    }

    protected void onTouchEvent(MotionEvent event) {

    }

    protected final void dispatchTouchEvent(MotionEvent event) {
        if(onTouchListener != null) {
            if(onTouchListener.onTouch(this, event))
                return;
        }
        if(event.action == MotionEvent.ACTION_DOWN) {
            isPressing = true;
            startX = event.x;
            startY = event.y;
        }
        onTouchEvent(event);
        switch(event.action) {
            case MotionEvent.ACTION_UP: {
                if(isPressing && onClickListener != null && containsPoint(event.x, event.y))
                    onClickListener.onClick(this);
                return;
            }
            case MotionEvent.ACTION_MOVE: {
                int diffX = event.x > startX ? event.x - startX : startX - event.x;
                int diffY = event.y > startY ? event.y - startY : startY - event.y;
                if(diffX > SCROLL_LIMIT || diffY > SCROLL_LIMIT || !containsPoint(event.x, event.y))
                    isPressing = false;
                return;
            }
        }
    }

    public int getWidth() {
        return width;
    }

    public int getHeight() {
        return height;
    }

    public Size getSize() {
        return new Size(width, height);
    }

    public void setSize(int width, int height) {
        this.width = width;
        this.height = height;
        invalidateLayout();
    }

    protected void updateLocation(int x, int y) {
        this.x = x;
        this.y = y;
    }

    public int getActualWidth() {
        return actualWidth;
    }

    public int getActualHeight() {
        return actualHeight;
    }

    public Size getActualSize() {
        return new Size(actualWidth, actualHeight);
    }

    protected void updateActualWidth(int availableW) {
        actualWidth = switch(width) {
            case View.MATCH_PARENT -> availableW >= 0 ? availableW : 0;
            case View.WRAP_CONTENT -> 0;
            default -> width;
        };
    }

    protected void updateActualHeight(int availableH) {
        actualHeight = switch(height) {
            case View.MATCH_PARENT -> availableH >= 0 ? availableH : 0;
            case View.WRAP_CONTENT -> 0;
            default -> height;
        };
    }

    public View getParent() {
        return parent;
    }

    protected boolean isVisible(Graphics g) {
        if(visible == false) return false;
        return g.isVisible(x, y, actualWidth, actualHeight);
    }

    public boolean getVisible() {
        return visible;
    }

    public void setVisible(boolean visible) {
        this.visible = visible;
        invalidateVisual();
    }

    public Margin getMargin() {
        return new Margin(marginLeft, marginTop, marginRight, marginBottom);
    }

    public void setMargin(int margin) {
        setMargin(margin, margin, margin, margin);
    }

    public void setMargin(Margin margin) {
        setMargin(margin.left, margin.top, margin.right, margin.bottom);
    }

    public void setMargin(int left, int top, int right, int bottom) {
        marginLeft = left;
        marginTop = top;
        marginRight = right;
        marginBottom = bottom;
        invalidateLayout();
    }

    public Object getBackground() {
        return background;
    }

    public void setBackground(Object bg) {
        if(bg == null)
            background = null;
        else if((bg instanceof Color) || (bg instanceof Image))
            background = bg;
        else
            throw new IllegalArgumentException("background must be an instance of Color or Image");
        invalidateVisual();
    }

    public HorizontalAlignment getHorizontalAlignment() {
        return hAlignment;
    }

    public void setHorizontalAlignment(HorizontalAlignment alignment) {
        if(alignment == null)
            throw new NullPointerException("alignment cannot be null");
        hAlignment = alignment;
        invalidateLayout();
    }

    public VerticalAlignment getVerticalAlignment() {
        return vAlignment;
    }

    public void setVerticalAlignment(VerticalAlignment alignment) {
        if(alignment == null)
            throw new NullPointerException("alignment cannot be null");
        vAlignment = alignment;
        invalidateLayout();
    }

    public void setOnClickListener(OnClickListener listener) {
        onClickListener = listener;
    }

    public void setOnTouchListener(OnTouchListener listener) {
        onTouchListener = listener;
    }
}
