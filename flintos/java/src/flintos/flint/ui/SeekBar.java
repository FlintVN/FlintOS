package flint.ui;

import flint.drawing.Image;
import flint.drawing.Color;
import flint.drawing.Graphics;

public class SeekBar extends View {
    private static final int ANIMATION_DURATION_1 = 150;
    private static final int ANIMATION_DURATION_2 = 500;
    private static final int PRESS_OFFSET = 2;
    private static final int PRESS_OFFSET_X2 = PRESS_OFFSET << 1;

    private static final int THUMB_SIZE = 20;
    private static final int TRACK_SIZE = 10;

    protected Color color;
    protected Color thumbColor;
    protected int max;
    protected int value;

    private int currValue;
    private int diffValue;
    private int startTime;

    private boolean isPressed;
    private boolean animStatus;
    private int animStartTime;

    private OnValueChangedListener onValueChangedListener;

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
        int value = isPressed ? currValue : valueWithAnimation();

        int x = this.x;
        int y = this.y;
        int w = actualWidth;
        int r = TRACK_SIZE / 2;

        int trackY = y + (THUMB_SIZE - TRACK_SIZE) / 2;
        int trackW = value + THUMB_SIZE / 2;
        g.fillRoundRect(color, x, trackY, trackW, TRACK_SIZE, r, 0, 0, r);
        g.fillRoundRect(background, x + trackW, trackY, w - trackW, TRACK_SIZE, 0, r, r, 0);

        int s = THUMB_SIZE;
        x += value;
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

    protected final int valueWithAnimation() {
        int target = (actualWidth - THUMB_SIZE) * this.value / max;
        int value = currValue;
        if (value != target) {
            value = calcCurrValue();
            if (value == target)
                currValue = value;
            invalidate(false);
        }

        return value;
    }

    private final int calcCurrValue() {
        int time = (int)System.currentTimeMillis() - startTime;
        if (time < ANIMATION_DURATION_2)
            return currValue + (diffValue * time / ANIMATION_DURATION_2);
        else
            return (actualWidth - THUMB_SIZE) * value / max;
    }

    @Override
    protected void invalidate(boolean layoutImpact) {
        FlintUI.setInvalidate(
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
    protected void updateActualWidth(int availableW) {
        currValue = calcCurrValue();

        super.updateActualWidth(availableW);

        int w = actualWidth - THUMB_SIZE;
        if (currValue > w) currValue = w;
        diffValue = (w * value / max) - currValue;
        startTime = (int)System.currentTimeMillis();
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
                    invalidate(false);
                }
                return;
            }
            case MotionEvent.ACTION_UP: {
                if (isPressed) {
                    isPressed = false;
                    animStatus = true;
                    animStartTime = (int)System.currentTimeMillis();
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
                        currValue = w * value / max;
                        if (onValueChangedListener != null)
                            onValueChangedListener.onValueChanged(this);
                        invalidate(false);
                    }
                }
                return;
            }
        }
    }

    private boolean hitThumb(int x) {
        int s = THUMB_SIZE * 3;
        int thumbX = this.x + currValue - THUMB_SIZE;
        return thumbX <= x && x <= (thumbX + s);
    }

    @Override
    public void setSize(int width, int height) {
        FlintUI.checkThread();
        this.width = width;
        this.height = View.WRAP_CONTENT;
        FlintUI.setInvalidateAll();
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
            if (value > max)
                value = max;
            int w = actualWidth - THUMB_SIZE;
            currValue = (currValue > w) ? w : calcCurrValue();
            diffValue = (w * value / max) - currValue;
            startTime = (int)System.currentTimeMillis();
            this.max = max;
        }
        invalidate(false);
    }

    public int getValue() {
        return value;
    }

    public void setValue(int value) {
        setValue(value, false);
    }

    public void setValue(int value, boolean animate) {
        FlintUI.checkThread();
        if (value < 0)
            value = 0;
        else if (value > max)
            value = max;
        if (this.value != value) {
            int w = actualWidth - THUMB_SIZE;
            if (!animate)
                currValue = w * value / max;
            else {
                currValue = calcCurrValue();
                diffValue = (w * value / max) - currValue;
                startTime = (int)System.currentTimeMillis();
            }
            this.value = value;
            if (onValueChangedListener != null)
                onValueChangedListener.onValueChanged(this);
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

    public void setOnValueChangedListener(OnValueChangedListener listener) {
        onValueChangedListener = listener;
    }
}
