package flint.ui;

public class Margin {
    public int left;
    public int top;
    public int right;
    public int bottom;

    public Margin() {

    }

    public Margin(int thickness) {
        this(thickness, thickness, thickness, thickness);
    }

    public Margin(int top, int left, int right, int bottom) {
        this.left = left;
        this.top = top;
        this.right = right;
        this.bottom = bottom;
    }
}
