package javax.microedition.lcdui;

import flint.drawing.Color;

/* MIDP Graphics mapped onto flint.drawing.Graphics (which rasterizes natively into the
 * target RGB565 buffer). Tracks color/font/clip/translate; image ops are added in the
 * lcdui.Image increment. */
public class Graphics {
    /* anchors */
    public static final int HCENTER = 1, VCENTER = 2, LEFT = 4, RIGHT = 8, TOP = 16, BOTTOM = 32, BASELINE = 64;
    /* stroke styles */
    public static final int SOLID = 0, DOTTED = 1;

    private final flint.drawing.Graphics fg;
    private final int bufW, bufH;

    private int color = 0;                       // 0xRRGGBB
    private Color fcolor = new Color(0, 0, 0);
    private Font font = Font.getDefaultFont();
    private int transX = 0, transY = 0;
    private int clipX, clipY, clipW, clipH;
    private int stroke = SOLID;

    Graphics(flint.drawing.Graphics fg, int w, int h) {
        this.fg = fg; this.bufW = w; this.bufH = h;
        this.clipX = 0; this.clipY = 0; this.clipW = w; this.clipH = h;
    }

    /* Reset to a fresh per-frame state (MIDP hands paint() an identity-transform,
     * full-clip Graphics each time). */
    void reset() {
        fg.setTransform(0, 0);
        transX = 0; transY = 0;
        setClip(0, 0, bufW, bufH);
        setColor(0);
        font = Font.getDefaultFont();
    }

    flint.drawing.Graphics flint() { return fg; }

    /* ---- color / font ---- */
    public void setColor(int rgb) {
        color = rgb & 0xFFFFFF;
        fcolor = new Color((color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF);
    }
    public void setColor(int r, int g, int b) { setColor((r << 16) | (g << 8) | b); }
    public int getColor() { return color; }
    public int getRedComponent() { return (color >> 16) & 0xFF; }
    public int getGreenComponent() { return (color >> 8) & 0xFF; }
    public int getBlueComponent() { return color & 0xFF; }
    public int getGrayScale() { return (getRedComponent() + getGreenComponent() + getBlueComponent()) / 3; }
    public void setGrayScale(int v) { setColor(v, v, v); }
    public void setStrokeStyle(int s) { stroke = s; }
    public int getStrokeStyle() { return stroke; }
    public void setFont(Font f) { if(f != null) font = f; }
    public Font getFont() { return font; }

    /* ---- transform / clip ----
     * MIDP clip coords are in the CURRENT (translated) user space, but the native
     * flint.drawing layer applies the translation only to draw ops (x+OX, y+OY) — it treats
     * the clip rectangle as absolute framebuffer coords. To keep the two consistent we store
     * the clip in DEVICE space (clipX/clipY already include the translation) and convert back
     * to user space in the getters. Without this, any clip set while a translation is active
     * landed transX/transY pixels away from the geometry it was meant to bound: harmless for
     * the menus (no translate) but wrong for the in-level renderer, which sets the clip and
     * then translates the camera before drawing — so the level clipped itself away. */
    public void translate(int x, int y) { transX += x; transY += y; fg.translate(x, y); }
    public int getTranslateX() { return transX; }
    public int getTranslateY() { return transY; }

    public void setClip(int x, int y, int w, int h) {
        clipX = x + transX; clipY = y + transY; clipW = w; clipH = h;   // fold translation in -> device space
        fg.setClip(clipX, clipY, w, h);
    }
    public void clipRect(int x, int y, int w, int h) {
        int dx = x + transX, dy = y + transY;                          // incoming rect -> device space
        int nx = Math.max(clipX, dx), ny = Math.max(clipY, dy);
        int nx2 = Math.min(clipX + clipW, dx + w), ny2 = Math.min(clipY + clipH, dy + h);
        clipX = nx; clipY = ny; clipW = Math.max(0, nx2 - nx); clipH = Math.max(0, ny2 - ny);
        fg.setClip(clipX, clipY, clipW, clipH);
    }
    public int getClipX() { return clipX - transX; }                   // device space -> user space
    public int getClipY() { return clipY - transY; }
    public int getClipWidth() { return clipW; }
    public int getClipHeight() { return clipH; }

    /* ---- primitives ---- */
    public void fillRect(int x, int y, int w, int h) { fg.fillRect(fcolor, x, y, w, h); }
    public void drawRect(int x, int y, int w, int h) { fg.drawRect(fcolor, 1, x, y, w, h); }
    public void drawLine(int x1, int y1, int x2, int y2) { fg.drawLine(fcolor, 1, x1, y1, x2, y2); }
    public void fillRoundRect(int x, int y, int w, int h, int aw, int ah) { fg.fillRoundRect(fcolor, x, y, w, h, aw, aw, aw, aw); }
    public void drawRoundRect(int x, int y, int w, int h, int aw, int ah) { fg.drawRoundRect(fcolor, 1, x, y, w, h, aw, aw, aw, aw); }
    public void fillArc(int x, int y, int w, int h, int sa, int aa) { fg.fillEllipse(fcolor, x, y, w, h); }
    public void drawArc(int x, int y, int w, int h, int sa, int aa) { fg.drawEllipse(fcolor, 1, x, y, w, h); }

    public void fillTriangle(int x1, int y1, int x2, int y2, int x3, int y3) {
        /* flint has no fillTriangle; outline for now (not on the game's hot path). */
        fg.drawLine(fcolor, 1, x1, y1, x2, y2);
        fg.drawLine(fcolor, 1, x2, y2, x3, y3);
        fg.drawLine(fcolor, 1, x3, y3, x1, y1);
    }

    public void drawString(String s, int x, int y, int anchor) { /* glyphs deferred (Font has metrics only) */ }
    public void drawSubstring(String s, int off, int len, int x, int y, int anchor) {}
    public void drawChar(char c, int x, int y, int anchor) {}
    public void drawChars(char[] data, int off, int len, int x, int y, int anchor) {}

    /* ---- images ---- */
    public void drawImage(Image img, int x, int y, int anchor) {
        if(img == null) return;
        int w = img.getWidth(), h = img.getHeight();
        x = anchorX(x, w, anchor);
        y = anchorY(y, h, anchor);
        fg.drawImage(img.raw(), x, y);
    }

    /* Sub-rectangle blit. Implemented for TRANS_NONE via clip + offset; transforms
     * (mirror/rotate) fall back to no-transform for now (game uses drawRegion once). */
    public void drawRegion(Image src, int sx, int sy, int sw, int sh,
                           int transform, int dx, int dy, int anchor) {
        if(src == null) return;
        dx = anchorX(dx, sw, anchor);
        dy = anchorY(dy, sh, anchor);
        int cx = getClipX(), cy = getClipY(), cw = clipW, ch = clipH;   // save clip (user space)
        clipRect(dx, dy, sw, sh);
        fg.drawImage(src.raw(), dx - sx, dy - sy);
        setClip(cx, cy, cw, ch);                                        // restore
    }

    private int anchorX(int x, int w, int anchor) {
        if((anchor & RIGHT) != 0) return x - w;
        if((anchor & HCENTER) != 0) return x - w / 2;
        return x;   // LEFT / default
    }
    private int anchorY(int y, int h, int anchor) {
        if((anchor & BOTTOM) != 0) return y - h;
        if((anchor & VCENTER) != 0) return y - h / 2;
        return y;   // TOP / BASELINE / default
    }
}
