package flint.ui;

import flint.drawing.Size;
import flint.drawing.Font;
import flint.drawing.Color;
import flint.drawing.ClipMode;
import flint.drawing.Graphics;

public class EditText extends PanelView {
    protected String text;
    protected Font font;
    protected Color textColor;

    protected int paddingLeft;
    protected int paddingTop;
    protected int paddingRight;
    protected int paddingBottom;

    public EditText() {
        background = Theme.defaultTheme.surfaceColor();
        borderColor = Theme.defaultTheme.borderColor();

        font = Theme.defaultTheme.defaultFont();
        textColor = Theme.defaultTheme.textColor();

        width = 120;
        height = 26;

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
        int x1 = thk + paddingLeft;
        int x2 = actualWidth - thk - paddingRight;

        g.setClip(this.x + x1, this.y + thk, x2 - x1, actualHeight - (thk << 1), ClipMode.INTERSECT);

        int txtW = Graphics.measureStringWidth(text, font);
        int txtH = Graphics.measureStringHeight(null, font);
        int spaceW = x2 - x1;
        int x = ((txtW > spaceW) ? (spaceW - txtW) : 0) + x1 + this.x;
        int y = (actualHeight - txtH) / 2 + paddingTop - paddingBottom + this.y;
        g.drawString(text, font, textColor, x, y);

        g.setClip(gClipX, gClipY, gClipW, gClipH, ClipMode.REPLACE);
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

            contentH += Graphics.measureStringHeight(null, font);

            actualHeight = height >= 0 ? height : ((height == View.WRAP_CONTENT || availableH < 0) ? contentH : availableH);
        }
        else
            actualHeight = height >= 0 ? height : availableH;
    }

    public String getText() {
        return text;
    }

    public void setText(String text) {
        FlintUI.checkThread();
        this.text = text;
        if(width == View.WRAP_CONTENT || height == View.WRAP_CONTENT)
            FlintUI.setInvalidateAll();
        else
            invalidate(false);
    }

    public Font getFont() {
        return font;
    }

    public void setFont(Font font) {
        FlintUI.checkThread();
        if(font == null)
            throw new NullPointerException("font cannot be null");
        this.font = font;
        if(width == View.WRAP_CONTENT || height == View.WRAP_CONTENT)
            FlintUI.setInvalidateAll();
        else
            invalidate(false);
    }

    public Color getTextColor() {
        return textColor;
    }

    public void setTextColor(Color color) {
        FlintUI.checkThread();
        if(color == null)
            throw new NullPointerException("color can not be null");
        textColor = color;
        invalidate(false);
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
        FlintUI.checkThread();
        if(left < 0 || top < 0 || right < 0 || bottom < 0)
            throw new IllegalArgumentException("TextView does not support padding with negative numbers");
        paddingLeft = left;
        paddingTop = top;
        paddingRight = right;
        paddingBottom = bottom;
        FlintUI.setInvalidateAll();
    }
}
