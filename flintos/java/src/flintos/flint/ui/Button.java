package flint.ui;

import flint.drawing.Size;
import flint.drawing.Font;
import flint.drawing.Color;
import flint.drawing.Image;
import flint.drawing.ClipMode;
import flint.drawing.Graphics;

public class Button extends PanelView {
    private static final int ANIMATION_DURATION = 150;
    private static final int PRESS_OFFSET = 2;
    private static final int PRESS_OFFSET_X2 = PRESS_OFFSET << 1;

    protected String text;
    protected Font font;
    protected Color textColor;

    protected int paddingLeft;
    protected int paddingTop;
    protected int paddingRight;
    protected int paddingBottom;

    protected Object pressedBackground;

    private boolean animStatus = false;
    private boolean isReleased = true;
    private int startTime;

    public Button() {
        background = Theme.defaultTheme.primaryColor();
        pressedBackground = Theme.defaultTheme.pressedColor();

        font = Theme.defaultTheme.defaultFont();
        textColor = Theme.defaultTheme.textColor();

        width = 65;
        height = 30;

        int defaultRadius = Theme.defaultTheme.cornerRadius();
        topLeftRadius = defaultRadius;
        topRightRadius = defaultRadius;
        bottomLeftRadius = defaultRadius;
        bottomRightRadius = defaultRadius;

        paddingLeft = 2;
        paddingTop = 2;
        paddingRight = 2;
        paddingBottom = 2;
    }

    @Override
    public void invalidateVisual() {
        FlintUI.setInvalidateVisual(x - PRESS_OFFSET, y - PRESS_OFFSET, actualWidth + PRESS_OFFSET_X2, actualHeight + PRESS_OFFSET_X2);
    }

    @Override
    protected void onDraw(Graphics g) {
        int r1 = topLeftRadius;
        int r2 = topRightRadius;
        int r3 = bottomLeftRadius;
        int r4 = bottomRightRadius;

        int x = this.x;
        int y = this.y;
        int w = this.actualWidth;
        int h = this.actualHeight;

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
            w += tmp << 1;
            h += tmp << 1;
            invalidateVisual();
        }
        else if(!isReleased) {
            x -= PRESS_OFFSET;
            y -= PRESS_OFFSET;
            w += PRESS_OFFSET_X2;
            h += PRESS_OFFSET_X2;
        }

        Object bg = isReleased ? background : pressedBackground;
        if(bg != null) {
            if(bg instanceof Color color) {
                if(color.getAlpha() > 0)
                    g.fillRoundRect(color, x, y, w, h, r1, r2, r3, r4);
            }
            else
                g.drawImage((Image)bg, x, y, w, h);
        }

        if(borderColor != null && borderColor.getAlpha() > 0)
            g.drawRoundRect(borderColor, x, y, w - 1, h - 1, r1, r2, r3, r4);

        if(text == null) return;

        int gClipX = g.getClipX();
        int gClipY = g.getClipY();
        int gClipW = g.getClipWidth();
        int gClipH = g.getClipHeight();

        int thk = getBorderThickness();
        int thk2 = thk << 1;
        g.setClip(x + thk, y + thk, w - thk2, h - thk2, ClipMode.INTERSECT);

        int txtW = Graphics.measureStringWidth(text, font);
        int txtH = Graphics.measureStringHeight(null, font);
        x += (w - txtW) / 2 + paddingLeft - paddingRight;
        y += (h - txtH) / 2 + paddingTop - paddingBottom;
        g.drawString(text, font, textColor, x, y);

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
                invalidateVisual();
                return;
            }
        }
    }

    @Override
    protected void updateActualWidth(int availableW) {
        if(width == View.WRAP_CONTENT || (width == View.MATCH_PARENT && availableW < 0)) {
            int contentW = getBorderThickness() << 1;

            if(paddingLeft > 0) contentW += paddingLeft;
            if(paddingRight > 0) contentW += paddingRight;

            contentW += Graphics.measureStringWidth(text, font);

            actualWidth = width >= 0 ? width : ((width == View.WRAP_CONTENT || availableW < 0) ? contentW : availableW);
        }
        else
            actualWidth = width >= 0 ? width : availableW;
    }

    @Override
    protected void updateActualHeight(int availableH) {
        if(height == View.WRAP_CONTENT || (height == View.MATCH_PARENT && availableH < 0)) {
            int contentH = getBorderThickness() << 1;

            if(paddingTop > 0) contentH += paddingTop;
            if(paddingBottom > 0) contentH += paddingBottom;

            contentH += Graphics.measureStringHeight(text, font);

            actualHeight = height >= 0 ? height : ((height == View.WRAP_CONTENT || availableH < 0) ? contentH : availableH);
        }
        else
            actualHeight = height >= 0 ? height : availableH;
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

    public Object getPressedBackground() {
        return pressedBackground;
    }

    public void setPressedBackground(Object bg) {
        if(bg == null)
            pressedBackground = null;
        else if((bg instanceof Color) || (bg instanceof Image))
            pressedBackground = bg;
        else
            throw new IllegalArgumentException("background must be an instance of Color or Image");
        if(!isReleased)
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
        paddingLeft = left;
        paddingTop = top;
        paddingRight = right;
        paddingBottom = bottom;
        invalidateLayout();
    }
}
