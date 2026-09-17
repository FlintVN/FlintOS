package flint.ui;

import flint.drawing.Color;
import flint.drawing.Image;
import flint.drawing.Graphics;

class PanelView extends View {
    protected Color borderColor;

    protected int topLeftRadius;
    protected int topRightRadius;
    protected int bottomLeftRadius;
    protected int bottomRightRadius;

    public PanelView() {

    }

    @Override
    protected void onDraw(Graphics g) {
        int r1 = topLeftRadius;
        int r2 = topRightRadius;
        int r3 = bottomLeftRadius;
        int r4 = bottomRightRadius;

        if(background != null) {
            if(background instanceof Color color) {
                if(color.getAlpha() > 0)
                    g.fillRoundRect(color, x, y, actualWidth, actualHeight, r1, r2, r3, r4);
            }
            else
                g.drawImage((Image)background, x, y, actualWidth, actualHeight);
        }

        if(borderColor != null && borderColor.getAlpha() > 0) {
            int w = actualWidth - 1;
            int h = actualHeight - 1;
            g.drawRoundRect(borderColor, x, y, w, h, r1, r2, r3, r4);
        }
    }

    @Override
    protected void updateActualWidth(int availableW) {
        actualWidth = width >= 0 ? width : ((width == View.WRAP_CONTENT || availableW < 0) ? 0 : availableW);
    }

    @Override
    protected void updateActualHeight(int availableH) {
        actualHeight = height >= 0 ? height : ((height == View.WRAP_CONTENT || availableH < 0) ? 0 : availableH);
    }

    final int getBorderThickness() {
        return (borderColor != null && borderColor.getAlpha() > 0) ? 1 : 0;
    }

    public Color getBorderColor() {
        return borderColor;
    }

    public void setBorderColor(Color color) {
        borderColor = color;
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
}
