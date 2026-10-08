package flint.ui;

import java.io.File;
import java.io.IOException;
import java.io.UncheckedIOException;
import flint.drawing.Image;
import flint.drawing.Color;
import flint.drawing.ClipMode;
import flint.drawing.Graphics;

public class ImageView extends View {
    protected Image image;
    protected int scaleType;

    public enum ScaleType {
        CENTER(0),
        CENTER_CROP(1),
        FIT_CENTER(2),
        FIT_XY(3);

        final int value;

        private ScaleType(int value) {
            this.value = value;
        }

        static ScaleType fromValue(int value) {
            return switch(value) {
                case 0 -> CENTER_CROP;
                case 1 -> FIT_CENTER;
                default -> FIT_XY;
            };
        }
    }

    public ImageView() {
        width = WRAP_CONTENT;
        height = WRAP_CONTENT;
        scaleType = ScaleType.FIT_CENTER.value;
    }

    @Override
    protected void onDraw(Graphics g) {
        Color bg = background;
        if(bg != null && bg.getAlpha() > 0)
            g.fillRect(bg, x, y, actualWidth, actualHeight);

        Image img = image;
        if(img == null)
            return;

        switch(scaleType) {
            case 0: {   /* CENTER */
                int gClipX = g.getClipX();
                int gClipY = g.getClipY();
                int gClipW = g.getClipWidth();
                int gClipH = g.getClipHeight();
                g.setClip(this.x, this.y, width, height, ClipMode.INTERSECT);

                int ix = this.x + (actualWidth - img.getWidth()) / 2;
                int iy = this.y + (actualHeight - img.getHeight()) / 2;
                g.drawImage(img, ix, iy);

                g.setClip(gClipX, gClipY, gClipW, gClipH, ClipMode.REPLACE);
                break;
            }
            case 1: {   /* CENTER_CROP */
                int gClipX = g.getClipX();
                int gClipY = g.getClipY();
                int gClipW = g.getClipWidth();
                int gClipH = g.getClipHeight();
                g.setClip(this.x, this.y, width, height, ClipMode.INTERSECT);

                int x, y, w, h;
                if(((long)actualWidth * img.getHeight()) > ((long)actualHeight * img.getWidth())) {
                    w = actualWidth;
                    h = img.getHeight() * actualWidth / img.getWidth();
                    x = this.x;
                    y = this.y + (actualHeight - h) / 2;
                }
                else {
                    w = img.getWidth() * actualHeight / img.getHeight();
                    h = actualHeight;
                    x = this.x + (actualWidth - w) / 2;
                    y = this.y;
                }
                g.drawImage(img, x, y, w, h);

                g.setClip(gClipX, gClipY, gClipW, gClipH, ClipMode.REPLACE);
                break;
            }
            case 2: {   /* FIT_CENTER */
                int x, y, w, h;
                if(((long)actualWidth * img.getHeight()) < ((long)actualHeight / img.getWidth())) {
                    w = actualWidth;
                    h = img.getHeight() * actualWidth / img.getWidth();
                    x = this.x;
                    y = this.y + (actualHeight - h) / 2;
                }
                else {
                    w = img.getWidth() * actualHeight / img.getHeight();
                    h = actualHeight;
                    x = this.x + (actualWidth - w) / 2;
                    y = this.y;
                }
                g.drawImage(img, x, y, w, h);
                break;
            }
            case 3: {   /* FIT_XY */
                g.drawImage(img, this.x, this.y, actualWidth, actualHeight);
                break;
            }
            default:
                break;
        }
    }

    @Override
    protected void updateActualWidth(int availableW) {
        if(width == View.WRAP_CONTENT || (width == View.MATCH_PARENT && availableW < 0)) {
            int contentW = (image != null) ? image.getWidth() : 0;
            actualWidth = width >= 0 ? width : ((width == View.WRAP_CONTENT || availableW < 0) ? contentW : availableW);
        }
        else
            actualWidth = width >= 0 ? width : availableW;
    }

    @Override
    protected void updateActualHeight(int availableH) {
        if(height == View.WRAP_CONTENT || (height == View.MATCH_PARENT && availableH < 0)) {
            int contentH = (image != null) ? image.getHeight() : 0;
            actualHeight = height >= 0 ? height : ((height == View.WRAP_CONTENT || availableH < 0) ? contentH : availableH);
        }
        else
            actualHeight = height >= 0 ? height : availableH;
    }

    public Image getImage() {
        return image;
    }

    public void setImage(Image img) {
        FlintUI.checkThread();
        image = img;
        invalidate(false);
    }

    public ScaleType getScaleType() {
        return ScaleType.fromValue(scaleType);
    }

    public void setScaleType(ScaleType type) {
        FlintUI.checkThread();
        if(type == null)
            throw new NullPointerException("type cannot be null");
        if(scaleType != type.value) {
            scaleType = type.value;
            invalidate(false);
        }
    }
}
