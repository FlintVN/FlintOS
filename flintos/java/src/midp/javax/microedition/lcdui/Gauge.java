package javax.microedition.lcdui;

public class Gauge extends Item {
    public static final int INDEFINITE = -1;
    public static final int CONTINUOUS_RUNNING = 2;
    public static final int INCREMENTAL_UPDATING = 3;
    private int value;
    private int max;
    private boolean interactive;

    public Gauge(String label, boolean interactive, int maxValue, int initialValue) {
        super(label);
        this.interactive = interactive;
        this.max = maxValue;
        this.value = initialValue;
    }

    public int getValue() {
        return value;
    }

    public void setValue(int v) {
        value = v;
    }

    public int getMaxValue() {
        return max;
    }

    public void setMaxValue(int m) {
        max = m;
    }

    public boolean isInteractive() {
        return interactive;
    }
}
