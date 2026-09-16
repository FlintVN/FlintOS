package flint.ui;

public enum Orientation {
    HORIZONTAL(0),
    VERTICAL(1),
    NONE(2);

    final int value;

    private Orientation(int value) {
        this.value = value;
    }
}
