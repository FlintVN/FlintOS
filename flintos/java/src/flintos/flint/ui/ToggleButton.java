package flint.ui;

import flint.drawing.Image;
import flint.drawing.Color;
import flint.drawing.ClipMode;
import flint.drawing.Graphics;

public class ToggleButton extends View {
    private static final int DEFAULT_HEIGHT = 24;
    private static final int DEFAULT_RADIUS = 12;

    protected boolean checked;

    protected Color onColor;
    protected Color thumbColor;

    protected Color borderColor;

    protected int cornerRadius;

    public ToggleButton() {
        onColor = Theme.defaultTheme.primaryColor();
        background = Theme.defaultTheme.disabledColor();
        thumbColor = Theme.defaultTheme.thumbColor();

        width = DEFAULT_HEIGHT * 2 - 4;
        height = DEFAULT_HEIGHT;

        cornerRadius = DEFAULT_RADIUS;
    }

    @Override
    protected void onDraw(Graphics g) {
        int r = cornerRadius;
        Object bgColor = checked ? onColor : background;
        if(bgColor != null)
            g.fillRoundRect((Color)bgColor, this.x, this.y, actualWidth, actualHeight, r, r, r, r);

        if(borderColor != null && borderColor.getAlpha() > 0) {
            int w = actualWidth - 1;
            int h = actualHeight - 1;
            g.drawRoundRect(borderColor, this.x, this.y, w, h, r, r, r, r);
        }

        Color c = thumbColor;
        if(c != null && c.getAlpha() > 0) {
            int thk = (borderColor != null && borderColor.getAlpha() > 0) ? 1 : 0;
            int h = actualHeight - (thk << 1) - 6;
            int y = thk + 3;
            int x = checked ? (actualWidth - y - h) : y;
            r = cornerRadius - thk - 3;
            g.fillRoundRect(c, this.x + x, this.y + y, h, h, r, r, r, r);
        }
    }

    @Override
    protected void onTouchEvent(MotionEvent event) {
        switch(event.action) {
            case MotionEvent.ACTION_UP:
                if(containsPoint(event.x, event.y)) {
                    checked = !checked;
                    if(onClickListener != null)
                        onClickListener.onClick(this);
                }
                break;
        }
    }

    @Override
    protected void updateActualWidth(int availableW) {
        if((width == View.WRAP_CONTENT) || (width == View.MATCH_PARENT && availableW < 0))
            actualWidth = width >= 0 ? width : ((width == View.WRAP_CONTENT || availableW < 0) ? (DEFAULT_HEIGHT * 2 - 4) : availableW);
        else
            actualWidth = width >= 0 ? width : availableW;
    }

    @Override
    protected void updateActualHeight(int availableH) {
        if((height == View.WRAP_CONTENT) || (height == View.MATCH_PARENT && availableH < 0))
            actualHeight = height >= 0 ? height : ((height == View.WRAP_CONTENT || availableH < 0) ? DEFAULT_HEIGHT : availableH);
        else
            actualHeight = height >= 0 ? height : availableH;
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

    public boolean isChecked() {
        return checked;
    }

    public void setChecked(boolean checked) {
        this.checked = checked;
    }

    public Color getOnColor() {
        return onColor;
    }

    public void setOnColor(Color color) {
        onColor = color;
    }

    public Color getThumbColor() {
        return thumbColor;
    }

    public void setThumbColor(Color color) {
        thumbColor = color;
    }

    public Color getBorderColor() {
        return borderColor;
    }

    public void setBorderColor(Color color) {
        borderColor = color;
    }

    public int getCornerRadius() {
        return cornerRadius;
    }

    public void setCornerRadius(int radius) {
        cornerRadius = radius;
    }
}
