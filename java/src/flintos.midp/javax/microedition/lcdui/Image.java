package javax.microedition.lcdui;

import flint.drawing.RawImage;
import flintos.midp.PngDecoder;
import flintos.midp.ResourceLoader;

/* MIDP Image backed by a flint.drawing.RawImage so it can be blitted with Flint's native
 * drawImage. Pixels are stored in Flint's BGR-packed big-endian RGB565 (same encoding as
 * the screen framebuffer / ColorRgb565), so a sprite copies straight onto the panel. */
public class Image {
    static final int IMG_RGB565 = 2, IMG_ARGB565 = 3;

    private final RawImage raw;
    private final int width, height;
    private final byte[] data;
    private final boolean mutable;

    private Image(int format, int w, int h, byte[] data, boolean mutable) {
        this.width = w; this.height = h; this.data = data; this.mutable = mutable;
        this.raw = new RawImage(format, w, h, data);
    }

    RawImage raw() { return raw; }

    /* ---- factories ---- */

    /** Mutable image, starts opaque white (per MIDP). */
    public static Image createImage(int w, int h) {
        byte[] d = new byte[w * h * 2];
        java.util.Arrays.fill(d, (byte) 0xFF);   // white = 0xFFFF
        return new Image(IMG_RGB565, w, h, d, true);
    }

    public static Image createImage(Image src) {
        byte[] d = new byte[src.data.length];
        System.arraycopy(src.data, 0, d, 0, d.length);
        return new Image(src.raw.format(), src.width, src.height, d, false);
    }

    public static Image createRGBImage(int[] rgb, int w, int h, boolean processAlpha) {
        if(processAlpha) {
            int aBytes = (w * h + 1) >> 1;
            byte[] d = new byte[w * h * 2 + aBytes];
            for(int i = 0; i < w * h; i++) {
                int argb = rgb[i];
                pack565(d, i, argb);
                int a4 = ((argb >>> 24) & 0xFF) >> 4;
                int ai = w * h * 2 + (i >> 1);
                if((i & 1) == 0) d[ai] = (byte) ((d[ai] & 0xF0) | a4);
                else              d[ai] = (byte) ((d[ai] & 0x0F) | (a4 << 4));
            }
            return new Image(IMG_ARGB565, w, h, d, false);
        }
        else {
            byte[] d = new byte[w * h * 2];
            for(int i = 0; i < w * h; i++) pack565(d, i, rgb[i]);
            return new Image(IMG_RGB565, w, h, d, false);
        }
    }

    /* PNG: decode natively (ROM miniz inflate) into Flint's RGB565/ARGB565 layout. Falls back
     * to a magenta placeholder of the right size if decode fails (unsupported format etc.). */
    public static Image createImage(byte[] imageData, int off, int len) {
        int[] wh = new int[3];
        byte[] px = PngDecoder.decode(imageData, off, len, wh);
        if(px != null)
            return new Image(wh[2], wh[0], wh[1], px, false);

        int w = 16, h = 16;
        if(len >= 24 && (imageData[off] & 0xFF) == 0x89) {   // PNG signature
            w = beInt(imageData, off + 16);                   // IHDR width
            h = beInt(imageData, off + 20);                   // IHDR height
        }
        if(w <= 0 || w > 2048) w = 16;
        if(h <= 0 || h > 2048) h = 16;
        byte[] d = new byte[w * h * 2];
        for(int i = 0; i < w * h; i++) { d[2 * i] = (byte) 0xF8; d[2 * i + 1] = (byte) 0x1F; } // magenta
        return new Image(IMG_RGB565, w, h, d, false);
    }
    public static Image createImage(java.io.InputStream stream) throws java.io.IOException {
        byte[] data = stream.readAllBytes();
        return createImage(data, 0, data.length);
    }
    public static Image createImage(String name) throws java.io.IOException {
        try (java.io.InputStream is = ResourceLoader.open(name)) {
            return createImage(is);
        }
    }
    private static int beInt(byte[] b, int o) {
        return ((b[o] & 0xFF) << 24) | ((b[o + 1] & 0xFF) << 16) | ((b[o + 2] & 0xFF) << 8) | (b[o + 3] & 0xFF);
    }

    /* Flint BGR-packed big-endian RGB565 (matches ColorRgb565). */
    private static void pack565(byte[] d, int i, int argb) {
        int r = (argb >> 16) & 0xFF, g = (argb >> 8) & 0xFF, b = argb & 0xFF;
        int v = ((b >> 3) << 11) | ((g >> 2) << 5) | (r >> 3);   // B high, R low
        d[2 * i]     = (byte) (v >> 8);
        d[2 * i + 1] = (byte) (v & 0xFF);
    }

    /* ---- instance ---- */
    public Graphics getGraphics() {
        if(!mutable) throw new IllegalStateException("immutable image");
        flint.drawing.Graphics fg = flint.drawing.Graphics.create(width, height, data);
        return new Graphics(fg, width, height);
    }
    public int getWidth() { return width; }
    public int getHeight() { return height; }
    public boolean isMutable() { return mutable; }
}
