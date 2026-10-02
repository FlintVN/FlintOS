package javax.microedition.lcdui;

import java.io.IOException;

public class Image {
    final flint.drawing.Image img;
    private final boolean mutable;

    private Image(flint.drawing.Image img, boolean mutable) {
        this.img = img;
        this.mutable = mutable;
    }

    public static Image createImage(int w, int h) {
        return new Image(flint.drawing.Image.create(w, h), true);
    }

    public static Image createImage(Image src) {
        return new Image(flint.drawing.Image.create(src.img), false);
    }

    public static Image createRGBImage(int[] rgb, int w, int h, boolean processAlpha) {
        return new Image(flint.drawing.Image.create(rgb, w, h, processAlpha), false);
    }

    public static Image createImage(byte[] imageData, int off, int len) {
        return new Image(flint.drawing.Image.create(imageData, off, len), false);
    }

    public static Image createImage(java.io.InputStream stream) throws IOException {
        return new Image(flint.drawing.Image.create(stream), false);
    }

    public static Image createImage(String name) throws IOException {
        try(java.io.InputStream is = Image.class.getResourceAsStream(name)) {
            if(is == null)
                throw new IOException("resource not found: " + name);
            return createImage(is);
        }
    }

    public Graphics getGraphics() {
        if(!mutable)
            throw new IllegalStateException("immutable image");
        return new Graphics(flint.drawing.Graphics.create(img));
    }

    public int getWidth() {
        return img.getWidth();
    }

    public int getHeight() {
        return img.getHeight();
    }

    public boolean isMutable() {
        return mutable;
    }
}
