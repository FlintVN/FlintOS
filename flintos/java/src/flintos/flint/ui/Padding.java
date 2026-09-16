package flint.ui;

public class Padding {
    public int left;
    public int top;
    public int right;
    public int bottom;

    public Padding() {

    }

    public Padding(int thickness) {
        this(thickness, thickness, thickness, thickness);
    }

    public Padding(int top, int left, int right, int bottom) {
        this.left = left;
        this.top = top;
        this.right = right;
        this.bottom = bottom;
    }
}
