package flint.ui;

import flint.drawing.Size;
import flint.drawing.Font;
import flint.drawing.Color;
import flint.drawing.ClipMode;
import flint.drawing.Graphics;

public class Button extends PanelView {
    protected String text;
    protected Font font;
    protected Color textColor;

    protected int paddingLeft;
    protected int paddingTop;
    protected int paddingRight;
    protected int paddingBottom;

    private Object backgroundOld;

    public Button() {
        background = Theme.defaultTheme.primaryColor();

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
    protected void onDraw(Graphics g) {
        super.onDraw(g);

        if(text == null) return;

        int gClipX = g.getClipX();
        int gClipY = g.getClipY();
        int gClipW = g.getClipWidth();
        int gClipH = g.getClipHeight();

        int thk = getBorderThickness();

        int x1 = thk;
        int y1 = thk;
        int x2 = actualWidth - thk;
        int y2 = actualHeight - thk;

        if(paddingLeft > 0) x1 += paddingLeft;
        if(paddingTop > 0) y1 += paddingTop;
        if(paddingRight > 0) x2 -= paddingRight;
        if(paddingBottom > 0) y2 -= paddingBottom;

        g.setClip(this.x + x1, this.y + y1, x2 - x1, y2 - y1, ClipMode.INTERSECT);

        int txtW = Graphics.measureStringWidth(text, font);
        int txtH = Graphics.measureStringHeight(null, font);
        int x = (actualWidth - txtW) / 2 + paddingLeft - paddingRight + this.x;
        int y = (actualHeight - txtH) / 2 + paddingTop - paddingBottom + this.y;
        g.drawString(text, font, textColor, x, y);

        g.setClip(gClipX, gClipY, gClipW, gClipH, ClipMode.REPLACE);
    }

    @Override
    protected void onTouchEvent(MotionEvent event) {
        if(onTouchListener != null) {
            if(onTouchListener.onTouch(this, event))
                return;
        }
        switch(event.action) {
            case MotionEvent.ACTION_DOWN: {
                backgroundOld = background;
                background = Theme.defaultTheme.pressedColor();
                invalidateVisual();
                return;
            }
            case MotionEvent.ACTION_UP: {
                background = backgroundOld;
                invalidateVisual();
                if(onClickListener != null && containsPoint(event.x, event.y))
                    onClickListener.onClick(this);
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
