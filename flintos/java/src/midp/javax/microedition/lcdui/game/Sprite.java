package javax.microedition.lcdui.game;

import javax.microedition.lcdui.Graphics;
import javax.microedition.lcdui.Image;

/* Animated image split into equal frames (row-major). Transforms are ignored for now
 * (TRANS_NONE only) — drawRegion handles the non-transformed blit. */
public class Sprite extends Layer {
    public static final int TRANS_NONE = 0;
    public static final int TRANS_MIRROR_ROT180 = 1;
    public static final int TRANS_MIRROR = 2;
    public static final int TRANS_ROT180 = 3;
    public static final int TRANS_MIRROR_ROT270 = 4;
    public static final int TRANS_ROT90 = 5;
    public static final int TRANS_ROT270 = 6;
    public static final int TRANS_MIRROR_ROT90 = 7;

    private final Image image;
    private final int fw;
    private final int fh;
    private final int cols;
    private final int frameCount;
    private int frame = 0;
    private int[] sequence;
    private int seqIndex = 0;

    public Sprite(Image image, int frameWidth, int frameHeight) {
        super(frameWidth, frameHeight);
        this.image = image;
        this.fw = frameWidth;
        this.fh = frameHeight;
        this.cols = Math.max(1, image.getWidth() / frameWidth);
        this.frameCount = cols * Math.max(1, image.getHeight() / frameHeight);
    }

    public Sprite(Image image) {
        this(image, image.getWidth(), image.getHeight());
    }

    public Sprite(Sprite s) {
        this(s.image, s.fw, s.fh);
        this.frame = s.frame;
        this.x = s.x;
        this.y = s.y;
    }

    public int getRawFrameCount() {
        return frameCount;
    }

    public int getFrameSequenceLength() {
        return sequence != null ? sequence.length : frameCount;
    }

    public void setFrame(int f) {
        frame = f;
    }

    public int getFrame() {
        return sequence != null ? seqIndex : frame;
    }

    public void setFrameSequence(int[] seq) {
        sequence = seq;
        seqIndex = 0;
    }

    public void nextFrame() {
        if(sequence != null)
            seqIndex = (seqIndex + 1) % sequence.length;
        else
            frame = (frame + 1) % frameCount;
    }

    public void prevFrame() {
        if(sequence != null)
            seqIndex = (seqIndex + sequence.length - 1) % sequence.length;
        else
            frame = (frame + frameCount - 1) % frameCount;
    }

    public void setTransform(int transform) {
        /* TRANS_NONE only for now */
    }

    public void defineReferencePixel(int x, int y) {

    }

    public void setRefPixelPosition(int x, int y) {
        this.x = x;
        this.y = y;
    }

    public void paint(Graphics g) {
        if(!visible)
            return;
        int idx = sequence != null ? sequence[seqIndex] : frame;
        int sx = (idx % cols) * fw, sy = (idx / cols) * fh;
        g.drawRegion(image, sx, sy, fw, fh, 0, x, y, Graphics.TOP | Graphics.LEFT);
    }

    public boolean collidesWith(Sprite s, boolean pixelLevel) {
        return false;
    }

    public boolean collidesWith(TiledLayer t, boolean pixelLevel) {
        return false;
    }

    public boolean collidesWith(Image img, int x, int y, boolean pixelLevel) {
        return false;
    }
}
