package flint.ui;

import flint.drawing.Size;
import flint.drawing.Font;
import flint.drawing.Color;
import flint.drawing.Image;
import flint.drawing.ClipMode;
import flint.drawing.Graphics;

public class CheckBox extends View {
    private static final int BOX_SIZE = 18;

    protected String text;
    protected Font font;
    protected Color textColor;

    protected boolean checked;

    protected Color color;

    protected int topLeftRadius;
    protected int topRightRadius;
    protected int bottomLeftRadius;
    protected int bottomRightRadius;

    protected int paddingLeft;
    protected int paddingTop;
    protected int paddingRight;
    protected int paddingBottom;

    public CheckBox() {
        font = Theme.defaultTheme.defaultFont();
        textColor = Theme.defaultTheme.textColor();

        color = Theme.defaultTheme.primaryColor();

        width = View.WRAP_CONTENT;
        height = View.WRAP_CONTENT;
    }

    @Override
    protected void onDraw(Graphics g) {
        if(background != null) {
            Color bgColor = (Color)background;
            g.fillRoundRect(bgColor, this.x, this.y, actualWidth, actualHeight, topLeftRadius, topRightRadius, bottomRightRadius, bottomLeftRadius);
        }

        int gClipX = g.getClipX();
        int gClipY = g.getClipY();
        int gClipW = g.getClipWidth();
        int gClipH = g.getClipHeight();

        int x1 = paddingLeft;
        int y1 = paddingTop;
        int x2 = actualWidth - paddingRight;
        int y2 = actualHeight - paddingBottom;

        g.setClip(this.x + x1, this.y + y1, x2 - x1, y2 - y1, ClipMode.INTERSECT);

        Color c = color;
        if(c != null && c.getAlpha() > 0) {
            int r = 4;
            g.drawRoundRect(c, this.x + paddingLeft, this.y + paddingTop, BOX_SIZE, BOX_SIZE, r, r, r, r);
            if(checked) {
                r -= 2;
                g.fillRoundRect(c, this.x + paddingLeft + 3, this.y + paddingTop + 3, BOX_SIZE - 5, BOX_SIZE - 5, r, r, r, r);
            }
        }

        if(text != null) {
            int x = paddingLeft + BOX_SIZE + 6 + this.x;
            int y = (BOX_SIZE - Graphics.measureStringHeight(null, font) + 1) / 2 + paddingTop + this.y;
            g.drawString(text, font, textColor, x, y);
        }

        g.setClip(gClipX, gClipY, gClipW, gClipH, ClipMode.REPLACE);
    }

    @Override
    protected void onTouchEvent(MotionEvent event) {
        if(onTouchListener != null) {
            if(onTouchListener.onTouch(this, event))
                return;
        }
        switch(event.action) {
            case MotionEvent.ACTION_UP: {
                if(containsPoint(event.x, event.y)) {
                    checked = !checked;
                    if(onClickListener != null)
                        onClickListener.onClick(this);
                }
                return;
            }
        }
    }

    @Override
    protected void updateActualWidth(int availableW) {
        if(width == View.WRAP_CONTENT || (width == View.MATCH_PARENT && availableW < 0)) {
            int contentW = paddingLeft + paddingRight;

            int strW = Graphics.measureStringWidth(text, font);
            contentW += strW + BOX_SIZE + 5;

            actualWidth = width >= 0 ? width : ((width == View.WRAP_CONTENT || availableW < 0) ? contentW : availableW);
        }
        else
            actualWidth = width >= 0 ? width : availableW;
    }

    @Override
    protected void updateActualHeight(int availableH) {
        if(height == View.WRAP_CONTENT || (height == View.MATCH_PARENT && availableH < 0)) {
            int contentH = paddingTop + paddingBottom;

            int strH = Graphics.measureStringHeight(text, font);
            contentH += strH > (BOX_SIZE + 1) ? strH : (BOX_SIZE + 1);

            actualHeight = height >= 0 ? height : ((height == View.WRAP_CONTENT || availableH < 0) ? contentH : availableH);
        }
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

    public String getText() {
        return text;
    }

    public void setText(String text) {
        this.text = text;
    }

    public Font getFont() {
        return font;
    }

    public void setFont(Font font) {
        if(font == null)
            throw new NullPointerException("font cannot be null");
        this.font = font;
    }

    public Color getTextColor() {
        return textColor;
    }

    public void setTextColor(Color color) {
        textColor = color;
    }

    public boolean isChecked() {
        return checked;
    }

    public void setChecked(boolean checked) {
        this.checked = checked;
    }

    public Color getColor() {
        return color;
    }

    public void setColor(Color color) {
        this.color = color;
    }

    public CornerRadius getCornerRadius() {
        return new CornerRadius(topLeftRadius, topRightRadius, bottomRightRadius, bottomLeftRadius);
    }

    public void setCornerRadius(int radius) {
        setCornerRadius(radius, radius, radius, radius);
    }

    public void setCornerRadius(CornerRadius radius) {
        setCornerRadius(radius.topLeft, radius.topRight, radius.bottomRight, radius.bottomLeft);
    }

    public void setCornerRadius(int topLeft, int topRight, int bottomRight, int bottomLeft) {
        this.topLeftRadius = topLeft;
        this.topRightRadius = topRight;
        this.bottomLeftRadius = bottomLeft;
        this.bottomRightRadius = bottomRight;
    }

    public Padding getPading() {
        return new Padding(paddingLeft, paddingTop, paddingRight, paddingBottom);
    }

    public void setPadding(int padding) {
        setPadding(padding, padding, padding, padding);
    }

    public void setPadding(Padding padding) {
        setPadding(padding.left, padding.top, padding.right, padding.bottom);
    }

    public void setPadding(int left, int top, int right, int bottom) {
        if(left < 0 || top < 0 || right < 0 || bottom < 0)
            throw new IllegalArgumentException("TextView does not support padding with negative numbers");
        paddingLeft = left;
        paddingTop = top;
        paddingRight = right;
        paddingBottom = bottom;
    }
}
