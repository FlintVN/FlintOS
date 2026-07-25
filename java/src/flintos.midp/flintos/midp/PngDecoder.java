package flintos.midp;

/**
 * Native PNG decoder bridge.
 *
 * <p>Replaces the legacy {@code board.Png} class.  Decodes PNG data into
 * Flint's RGB565/ARGB565 pixel layout.</p>
 *
 * <p>Native methods are registered as
 * {@code NATIVE_CLASS("flintos/midp/PngDecoder", ...)}.</p>
 */
public final class PngDecoder {
    /**
     * Decode a PNG image.
     *
     * @param data raw PNG bytes
     * @param off  start offset within {@code data}
     * @param len  number of bytes to decode
     * @param wh   output array: {@code wh[0]=width, wh[1]=height,
     *             wh[2]=format (2=RGB565, 3=ARGB565)}
     * @return pixel buffer in Flint's RGB565/ARGB565 layout, or {@code null}
     *         on failure
     */
    public static native byte[] decode(byte[] data, int off, int len, int[] wh);

    private PngDecoder() {}
}
