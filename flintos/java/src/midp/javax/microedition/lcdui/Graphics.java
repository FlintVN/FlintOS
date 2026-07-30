package javax.microedition.lcdui;

import flint.drawing.Color;
import flint.drawing.ClipMode;

public class Graphics {
    public static final int HCENTER = 1;
    public static final int VCENTER = 2;
    public static final int LEFT = 4;
    public static final int RIGHT = 8;
    public static final int TOP = 16;
    public static final int BOTTOM = 32;
    public static final int BASELINE = 64;

    public static final int SOLID = 0;
    public static final int DOTTED = 1;

    private final flint.drawing.Graphics fg;

    private Color color = new Color(0, 0, 0);
    private Font font = Font.getDefaultFont();
    private int stroke = SOLID;

    Graphics(flint.drawing.Graphics fg) {
        this.fg = fg;
    }

    void reset() {
        fg.setTransform(0, 0);
        fg.resetClip();
        setColor(0);
        font = Font.getDefaultFont();
    }

    public void setColor(int rgb) {
        color = new Color(rgb);
    }

    public void setColor(int r, int g, int b) {
        color = new Color(r, g, b);
    }

    public int getColor() {
        return color.getValue() & 0xFFFFFF;
    }

    public int getRedComponent() {
        return color.getRed();
    }

    public int getGreenComponent() {
        return color.getGreen();
    }

    public int getBlueComponent() {
        return color.getBlue();
    }

    public int getGrayScale() {
        return (color.getRed() + color.getGreen() + color.getBlue()) / 3;
    }

    public void setGrayScale(int v) {
        setColor(v, v, v);
    }

    public void setStrokeStyle(int s) {
        stroke = s;
    }

    public int getStrokeStyle() {
        return stroke;
    }

    public void setFont(Font f) {
        if(f != null)
            font = f;
    }

    public Font getFont() {
        return font;
    }

    public void translate(int x, int y) {
        fg.translate(x, y);
    }

    public int getTranslateX() {
        return fg.getX();
    }

    public int getTranslateY() {
        return fg.getY();
    }

    public void setClip(int x, int y, int w, int h) {
        fg.setClip(x, y, w, h);
    }

    public void clipRect(int x, int y, int w, int h) {
        fg.setClip(x, y, w, h, ClipMode.INTERSECT);
    }

    public int getClipX() {
        return fg.getClipX();
    }

    public int getClipY() {
        return fg.getClipY();
    }

    public int getClipWidth() {
        return fg.getClipWidth();
    }

    public int getClipHeight() {
        return fg.getClipHeight();
    }

    public void fillRect(int x, int y, int w, int h) {
        fg.fillRect(color, x, y, w, h);
    }

    public void drawRect(int x, int y, int w, int h) {
        fg.drawRect(color, 1, x, y, w, h);
    }

    public void drawLine(int x1, int y1, int x2, int y2) {
        fg.drawLine(color, 1, x1, y1, x2, y2);
    }

    public void fillRoundRect(int x, int y, int w, int h, int aw, int ah) {
        fg.fillRoundRect(color, x, y, w, h, aw, aw, aw, aw);
    }

    public void drawRoundRect(int x, int y, int w, int h, int aw, int ah) {
        fg.drawRoundRect(color, 1, x, y, w, h, aw, aw, aw, aw);
    }

    public void fillArc(int x, int y, int w, int h, int sa, int aa) {
        fg.fillEllipse(color, x, y, w, h);
    }

    public void drawArc(int x, int y, int w, int h, int sa, int aa) {
        fg.drawEllipse(color, 1, x, y, w, h);
    }


    public void fillTriangle(int x1, int y1, int x2, int y2, int x3, int y3) {
        // TODO
        fg.drawLine(color, 1, x1, y1, x2, y2);
        fg.drawLine(color, 1, x2, y2, x3, y3);
        fg.drawLine(color, 1, x3, y3, x1, y1);
    }

    public void drawString(String s, int x, int y, int anchor) {
        /* glyphs deferred (Font has metrics only) */
    }

    public void drawSubstring(String s, int off, int len, int x, int y, int anchor) {

    }

    public void drawChar(char c, int x, int y, int anchor) {

    }

    public void drawChars(char[] data, int off, int len, int x, int y, int anchor) {

    }

    public void drawImage(Image img, int x, int y, int anchor) {
        if(img == null)
            return;
        int w = img.getWidth(), h = img.getHeight();
        x = anchorX(x, w, anchor);
        y = anchorY(y, h, anchor);
        fg.drawImage(img, x, y);
    }

    public void drawRegion(Image src, int sx, int sy, int sw, int sh, int transform, int dx, int dy, int anchor) {
        if(src == null)
            return;
        dx = anchorX(dx, sw, anchor);
        dy = anchorY(dy, sh, anchor);
        int cx = fg.getClipX();
        int cy = fg.getClipY();
        int cw = fg.getClipWidth();
        int ch = fg.getClipHeight();
        clipRect(dx, dy, sw, sh);
        fg.drawImage(src, dx - sx, dy - sy);
        setClip(cx, cy, cw, ch);
    }

    private int anchorX(int x, int w, int anchor) {
        if((anchor & RIGHT) != 0)
            return x - w;
        if((anchor & HCENTER) != 0)
            return x - w / 2;
        return x;
    }

    private int anchorY(int y, int h, int anchor) {
        if((anchor & BOTTOM) != 0)
            return y - h;
        if((anchor & VCENTER) != 0)
            return y - h / 2;
        return y;
    }
}
