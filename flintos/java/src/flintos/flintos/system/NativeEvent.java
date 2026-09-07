package flintos.system;

public class NativeEvent extends Event {
    private int[] data;

    public NativeEvent() {
        super(0);
        data = new int[4];
    }

    public int getData(int index) {
        return data[index];
    }
}
