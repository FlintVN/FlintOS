package flint.ui;

import flint.drawing.Color;
import flint.drawing.Graphics;

public class SeekBar extends View {
    private static final int ANIMATION_DURATION_1 = 150;
    private static final int PRESS_OFFSET = 2;
    private static final int PRESS_OFFSET_X2 = PRESS_OFFSET << 1;

    private static final int THUMB_SIZE = 20;
    private static final int TRACK_SIZE = 10;

    private Color color;
    private Color thumbColor;
    private int max;
    private int value;

    private boolean isPressed;
    private boolean animStatus;
    private int animStartTime;

    private OnSeekBarChangeListener onSeekBarChangeListener;

    public SeekBar() {
        max = 100;

        width = View.MATCH_PARENT;
        height = View.WRAP_CONTENT;

        color = Theme.getProperty("primaryColor");
        thumbColor = Theme.getProperty("thumbColor");
        background = Theme.getProperty("surfaceColor");
    }

    @Override
    protected void onDraw(Graphics g) {
        int v = (actualWidth - THUMB_SIZE) * value / max;

        int x = this.x;
        int y = this.y;
        int w = actualWidth;
        int r = TRACK_SIZE / 2;

        int trackY = y + (THUMB_SIZE - TRACK_SIZE) / 2;
        int trackW = v + THUMB_SIZE / 2;
        g.fillRoundRect(color, x, trackY, trackW, TRACK_SIZE, r, 0, 0, r);
        g.fillRoundRect(background, x + trackW, trackY, w - trackW, TRACK_SIZE, 0, r, r, 0);

        int s = THUMB_SIZE;
        x += v;
        if (animStatus) {
            int tmp, time = (int)System.currentTimeMillis() - animStartTime;
            if (time < ANIMATION_DURATION_1)
                tmp = time * PRESS_OFFSET / ANIMATION_DURATION_1;
            else {
                tmp = PRESS_OFFSET;
                animStatus = false;
            }
            if (!isPressed) tmp = PRESS_OFFSET - tmp;
            x -= tmp;
            y -= tmp;
            s += tmp << 1;
            invalidate(false);
        }
        else if (isPressed) {
            x -= PRESS_OFFSET;
            y -= PRESS_OFFSET;
            s += PRESS_OFFSET_X2;
        }

        g.fillEllipse(thumbColor, x, y, s, s);
    }

    @Override
    protected void invalidate(boolean layoutImpact) {
        invalidate(
            x - PRESS_OFFSET,
            y - PRESS_OFFSET,
            actualWidth + PRESS_OFFSET_X2,
            actualHeight + PRESS_OFFSET_X2,
            layoutImpact
        );
    }

    @Override
    protected boolean manipulationMode() {
        return true;
    }

    @Override
    protected void updateActualHeight(int availableH) {
        actualHeight = THUMB_SIZE;
    }

    @Override
    protected void onTouchEvent(MotionEvent event) {
        switch (event.action) {
            case MotionEvent.ACTION_DOWN: {
                if (hitThumb(event.x)) {
                    animStatus = true;
                    isPressed = true;
                    animStartTime = (int)System.currentTimeMillis();
                    if (onSeekBarChangeListener != null)
                        onSeekBarChangeListener.onStartTrackingTouch(this);
                    invalidate(false);
                }
                return;
            }
            case MotionEvent.ACTION_UP: {
                if (isPressed) {
                    isPressed = false;
                    animStatus = true;
                    animStartTime = (int)System.currentTimeMillis();
                    if (onSeekBarChangeListener != null)
                        onSeekBarChangeListener.onStopTrackingTouch(this);
                    invalidate(false);
                }
                return;
            }
            case MotionEvent.ACTION_MOVE: {
                if (isPressed) {
                    int w = actualWidth - THUMB_SIZE;
                    int v = event.x - this.x - THUMB_SIZE / 2;
                    if (v < 0) v = 0;
                    else if (v > w) v = w;
                    v = v * max / w;
                    if (value != v) {
                        value = v;
                        if (onSeekBarChangeListener != null)
                            onSeekBarChangeListener.onProgressChanged(this, true);
                        invalidate(false);
                    }
                }
                return;
            }
        }
    }

    private boolean hitThumb(int x) {
        int s = THUMB_SIZE * 3;
        int v = (actualWidth - THUMB_SIZE) * value / max;
        int thumbX = this.x + v - THUMB_SIZE;
        return thumbX <= x && x <= (thumbX + s);
    }

    @Override
    public void setSize(int width, int height) {
        FlintUI.checkThread();
        this.width = width;
        this.height = View.WRAP_CONTENT;
        invalidateAll();
    }

    public Color getColor() {
        return color;
    }

    public void setColor(Color color) {
        FlintUI.checkThread();
        if (color == null)
            throw new NullPointerException("color can not be null");
        this.color = color;
        invalidate(false);
    }

    public int getMax() {
        return max;
    }

    public void setMax(int max) {
        FlintUI.checkThread();
        if (max < 0)
            max = 1;
        if (this.max != max) {
            if (value > max) {
                value = max;
                if (onSeekBarChangeListener != null)
                    onSeekBarChangeListener.onProgressChanged(this, false);
            }
            this.max = max;
        }
        invalidate(false);
    }

    public int getValue() {
        return value;
    }

    public void setValue(int value) {
        FlintUI.checkThread();
        if (value < 0)
            value = 0;
        else if (value > max)
            value = max;
        if (this.value != value) {
            this.value = value;
            if (onSeekBarChangeListener != null)
                onSeekBarChangeListener.onProgressChanged(this, false);
            invalidate(false);
        }
    }

    public Color getThumbColor() {
        return thumbColor;
    }

    public void setThumbColor(Color color) {
        FlintUI.checkThread();
        if (color == null)
            throw new NullPointerException("color can not be null");
        thumbColor = color;
        invalidate(false);
    }

    public void setOnSeekBarChangeListener(OnSeekBarChangeListener listener) {
        onSeekBarChangeListener = listener;
    }
}
