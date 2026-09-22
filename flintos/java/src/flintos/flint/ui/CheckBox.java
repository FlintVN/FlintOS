package flint.ui;

import flint.drawing.Size;
import flint.drawing.Font;
import flint.drawing.Color;
import flint.drawing.Image;
import flint.drawing.ClipMode;
import flint.drawing.Graphics;

public class CheckBox extends View {
    private static final int BOX_SIZE = 18;
    private static final int ANIMATION_DURATION = 150;
    private static final int PRESS_OFFSET = 2;
    private static final int PRESS_OFFSET_X2 = PRESS_OFFSET << 1;

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

    private OnCheckedChangeListener onCheckedChangeListener;

    private boolean animStatus = false;
    private boolean isReleased = true;
    private int startTime;

    public CheckBox() {
        font = Theme.defaultTheme.defaultFont();
        textColor = Theme.defaultTheme.textColor();

        color = Theme.defaultTheme.primaryColor();

        width = View.WRAP_CONTENT;
        height = View.WRAP_CONTENT;
    }

    @Override
    public void invalidateVisual() {
        FlintUI.setInvalidateVisual(x - PRESS_OFFSET, y - PRESS_OFFSET, actualWidth + PRESS_OFFSET_X2, actualHeight + PRESS_OFFSET_X2);
    }

    @Override
    protected void onDraw(Graphics g) {
        if(background != null) {
            Color bgColor = (Color)background;
            g.fillRoundRect(bgColor, this.x, this.y, actualWidth, actualHeight, topLeftRadius, topRightRadius, bottomRightRadius, bottomLeftRadius);
        }

        Color c = color;
        if(c != null && c.getAlpha() > 0) {
            int r = 4;

            int x = this.x + paddingLeft;
            int y = this.y + paddingTop;
            int boxSize = BOX_SIZE;

            if(animStatus) {
                int tmp, time = (int)System.currentTimeMillis() - startTime;
                if(time < ANIMATION_DURATION)
                    tmp = time * PRESS_OFFSET / ANIMATION_DURATION;
                else {
                    tmp = PRESS_OFFSET;
                    animStatus = false;
                }
                if(isReleased) tmp = PRESS_OFFSET - tmp;
                x -= tmp;
                y -= tmp;
                boxSize += tmp << 1;
                invalidateVisual();
            }
            else if(!isReleased) {
                x -= PRESS_OFFSET;
                y -= PRESS_OFFSET;
                boxSize += PRESS_OFFSET_X2;
            }

            g.drawRoundRect(c, x, y, boxSize, boxSize, r, r, r, r);
            if(checked) {
                r -= 2;
                g.fillRoundRect(c, x + 3, y + 3, boxSize - 5, boxSize - 5, r, r, r, r);
            }
        }

        int gClipX = g.getClipX();
        int gClipY = g.getClipY();
        int gClipW = g.getClipWidth();
        int gClipH = g.getClipHeight();
        g.setClip(this.x, this.y, actualWidth, actualHeight, ClipMode.INTERSECT);

        if(text != null) {
            int x = paddingLeft + BOX_SIZE + 6 + this.x;
            int y = (BOX_SIZE - Graphics.measureStringHeight(null, font) + 1) / 2 + paddingTop + this.y;
            g.drawString(text, font, textColor, x, y);
        }

        g.setClip(gClipX, gClipY, gClipW, gClipH, ClipMode.REPLACE);
    }

    @Override
    protected void onTouchEvent(MotionEvent event) {
        switch(event.action) {
            case MotionEvent.ACTION_DOWN: {
                animStatus = true;
                isReleased = false;
                startTime = (int)System.currentTimeMillis();
                invalidateVisual();
                return;
            }
            case MotionEvent.ACTION_UP: {
                animStatus = true;
                isReleased = true;
                startTime = (int)System.currentTimeMillis();
                if(isPressing && containsPoint(event.x, event.y)) {
                    checked = !checked;
                    if(onCheckedChangeListener != null)
                        onCheckedChangeListener.onCheckedChanged(this, checked);
                }
                invalidateVisual();
                return;
            }
        }
    }

    @Override
    protected void updateActualWidth(int availableW) {
        if(width == View.WRAP_CONTENT || (width == View.MATCH_PARENT && availableW < 0)) {
            int contentW = paddingLeft + paddingRight;

            int strW = Graphics.measureStringWidth(text, font);
            contentW += strW + BOX_SIZE + 6;

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
        invalidateVisual();
    }

    public String getText() {
        return text;
    }

    public void setText(String text) {
        this.text = text;
        if(width == View.WRAP_CONTENT || height == View.WRAP_CONTENT)
            invalidateLayout();
        else
            invalidateVisual();
    }

    public Font getFont() {
        return font;
    }

    public void setFont(Font font) {
        if(font == null)
            throw new NullPointerException("font cannot be null");
        this.font = font;
        if(width == View.WRAP_CONTENT || height == View.WRAP_CONTENT)
            invalidateLayout();
        else
            invalidateVisual();
    }

    public Color getTextColor() {
        return textColor;
    }

    public void setTextColor(Color color) {
        textColor = color;
        invalidateVisual();
    }

    public boolean isChecked() {
        return checked;
    }

    public void setChecked(boolean checked) {
        if(this.checked != checked) {
            this.checked = checked;
            if(onCheckedChangeListener != null)
                onCheckedChangeListener.onCheckedChanged(this, checked);
            invalidateVisual();
        }
    }

    public Color getColor() {
        return color;
    }

    public void setColor(Color color) {
        this.color = color;
        invalidateVisual();
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
        invalidateVisual();
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
        invalidateLayout();
    }

    public void setOnCheckedChangeListener(OnCheckedChangeListener listener) {
        onCheckedChangeListener = listener;
    }
}
