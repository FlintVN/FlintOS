package flint.ui;

public class CornerRadius {
    public int topLeft;
    public int topRight;
    public int bottomLeft;
    public int bottomRight;

    public CornerRadius() {

    }

    public CornerRadius(int radius) {
        this(radius, radius, radius, radius);
    }

    public CornerRadius(int topLeft, int topRight, int bottomRight, int bottomLeft) {
        this.topLeft = topLeft;
        this.topRight = topRight;
        this.bottomLeft = bottomLeft;
        this.bottomRight = bottomRight;
    }
}
