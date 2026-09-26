package flint.ui;

import flint.drawing.Color;
import flint.drawing.Graphics;

public class ProgressBar extends PanelView {
    private static final int ANIMATION_DURATION = 500;

    protected Color color;
    protected int max;
    protected int value;

    private int currValue;
    private int diffValue;

    private int startTime;

    public ProgressBar() {
        color = Theme.defaultTheme.primaryColor();
        background = Theme.defaultTheme.surfaceColor();

        max = 100;

        width = 200;
        height = 30;

        int defaultRadius = Theme.defaultTheme.cornerRadius();
        topLeftRadius = defaultRadius;
        topRightRadius = defaultRadius;
        bottomLeftRadius = defaultRadius;
        bottomRightRadius = defaultRadius;
    }

    @Override
    protected void onDraw(Graphics g) {
        super.onDraw(g);

        int value = valueWithAnimation();

        int thk = getBorderThickness();
        g.fillRoundRect(
            color,
            this.x + thk, this.y + thk,
            value, actualHeight - (thk << 1),
            topLeftRadius - thk, topRightRadius - thk, bottomLeftRadius - thk, bottomRightRadius - thk
        );
    }

    protected final int valueWithAnimation() {
        int thk = getBorderThickness();
        int w = actualWidth - (thk << 1);

        int target = w * this.value / max;
        int value = currValue;
        if(value != target) {
            value = calcCurrValue();
            if(value == target)
                currValue = value;
            invalidate(false);
        }

        return value;
    }

    private final int calcCurrValue() {
        int time = (int)System.currentTimeMillis() - startTime;
        if(time < ANIMATION_DURATION)
            return currValue + (diffValue * time / ANIMATION_DURATION);
        else {
            int w = actualWidth - (getBorderThickness() << 1);
            return w * value / max;
        }
    }

    @Override
    protected void updateActualWidth(int availableW) {
        currValue = calcCurrValue();

        super.updateActualWidth(availableW);

        int w = actualWidth - (getBorderThickness() << 1);
        if(currValue > w) currValue = w;
        diffValue = (w * value / max) - currValue;
        startTime = (int)System.currentTimeMillis();
    }

    public Color getColor() {
        return color;
    }

    public void setColor(Color color) {
        FlintUI.checkThread();
        if(color == null)
            throw new NullPointerException("color can not be null");
        this.color = color;
        invalidate(false);
    }

    public int getMax() {
        return max;
    }

    public void setMax(int max) {
        FlintUI.checkThread();
        if(max < 0)
            max = 1;
        if(this.max != max) {
            if(value > max)
                value = max;
            int w = actualWidth - (getBorderThickness() << 1);
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
        if(value < 0)
            value = 0;
        else if(value > max)
            value = max;
        if(this.value != value) {
            int w = actualWidth - (getBorderThickness() << 1);
            if(!animate)
                currValue = w * value / max;
            else {
                currValue = calcCurrValue();
                diffValue = (w * value / max) - currValue;
                startTime = (int)System.currentTimeMillis();
            }
            this.value = value;
            invalidate(false);
        }
    }
}
