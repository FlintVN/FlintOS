package flint.ui;

import flint.drawing.Size;
import flint.drawing.Font;
import flint.drawing.Color;
import flint.drawing.Image;
import flint.drawing.ClipMode;
import flint.drawing.Graphics;

public class TextView extends View {
    protected String text;
    protected Font font;
    protected Color textColor;

    protected int topLeftRadius;
    protected int topRightRadius;
    protected int bottomLeftRadius;
    protected int bottomRightRadius;

    protected int paddingLeft;
    protected int paddingTop;
    protected int paddingRight;
    protected int paddingBottom;

    public TextView() {
        width = View.WRAP_CONTENT;
        height = View.WRAP_CONTENT;
        font = Theme.defaultTheme.defaultFont();
        textColor = Theme.defaultTheme.textColor();
    }

    @Override
    protected void onDraw(Graphics g) {
        if(background != null) {
            if(background instanceof Color color) {
                if(color.getAlpha() > 0)
                    g.fillRoundRect(color, x, y, actualWidth, actualHeight, topLeftRadius, topRightRadius, bottomRightRadius, bottomLeftRadius);
            }
            else
                g.drawImage((Image)background, x, y, actualWidth, actualHeight);
        }

        if(text == null || textColor == null) return;

        int gClipX = g.getClipX();
        int gClipY = g.getClipY();
        int gClipW = g.getClipWidth();
        int gClipH = g.getClipHeight();
        g.setClip(this.x, this.y, actualWidth, actualHeight, ClipMode.INTERSECT);

        g.drawString(text, font, textColor, x + paddingLeft, y + paddingTop);
    
        g.setClip(gClipX, gClipY, gClipW, gClipH, ClipMode.REPLACE);
    }
    
    @Override
    protected void updateActualWidth(int availableW) {
        if(width == View.WRAP_CONTENT || (width == View.MATCH_PARENT && availableW < 0)) {
            int contentW = Graphics.measureStringWidth(text, font) + paddingLeft + paddingRight;
            if(contentW < 0) contentW = 0;
            actualWidth = width >= 0 ? width : ((width == View.WRAP_CONTENT || availableW < 0) ? contentW : availableW);
        }
        else
            actualWidth = width >= 0 ? width : availableW;
    }

    @Override
    protected void updateActualHeight(int availableH) {
        if(height == View.WRAP_CONTENT || (height == View.MATCH_PARENT && availableH < 0)) {
            int contentH = Graphics.measureStringHeight(text, font) + paddingTop + paddingBottom;
            if(contentH < 0) contentH = 0;
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
        FlintUI.checkThread();
        this.topLeftRadius = topLeft;
        this.topRightRadius = topRight;
        this.bottomLeftRadius = bottomLeft;
        this.bottomRightRadius = bottomRight;
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
        paddingLeft = left;
        paddingTop = top;
        paddingRight = right;
        paddingBottom = bottom;
        FlintUI.setInvalidateAll();
    }
}
