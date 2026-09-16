package flint.ui;

import flint.drawing.Size;
import flint.drawing.Color;
import flint.drawing.Image;
import flint.drawing.Graphics;

public abstract class View {
    public static final int MATCH_PARENT = -1;
    public static final int WRAP_CONTENT = -2;

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

    public View() {

    }

    protected abstract void onDraw(Graphics g);

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
    }

    public HorizontalAlignment getHorizontalAlignment() {
        return hAlignment;
    }

    public void setHorizontalAlignment(HorizontalAlignment alignment) {
        if(alignment == null)
            throw new NullPointerException("alignment cannot be null");
        hAlignment = alignment;
    }

    public VerticalAlignment getVerticalAlignment() {
        return vAlignment;
    }

    public void setVerticalAlignment(VerticalAlignment alignment) {
        if(alignment == null)
            throw new NullPointerException("alignment cannot be null");
        vAlignment = alignment;
    }
}
