package flint.drawing;

/* Concrete flint.drawing.Image backed by an in-memory buffer, so MIDP mutable/RGB
 * images can be blitted via Flint's native drawImage. Package-split into flint.drawing
 * to set the package-private format/width/height/data fields the native binding reads
 * (field indices 0..3 in flint_native_rgb565_graphics.cpp).
 *
 * format: 2 = IMG_RGB565 (w*h big-endian 16bpp), 3 = IMG_ARGB565 (RGB565 plane + 4-bit
 * alpha plane). Both use Flint's BGR-packed RGB565 (see ColorRgb565). */
public final class RawImage extends Image {
    public RawImage(int format, int w, int h, byte[] data) {
        super(null);              // Image(String) ctor body is empty
        this.format = format;
        this.width  = w;
        this.height = h;
        this.data   = data;
    }
    public byte[] data() { return data; }
    public int format() { return format; }
}
