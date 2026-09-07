package flintos.system;

public class Event {
    private int type;

    protected Event(int eventType) {
        type = eventType;
    }

    public int getType() {
        return type;
    }
}
