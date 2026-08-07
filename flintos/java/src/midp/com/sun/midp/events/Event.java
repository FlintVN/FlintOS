package com.sun.midp.events;

public class Event {
    int type;
    Event next;

    public Event(int eventType) {
        type = eventType;
    }

    public int getType() {
        return type;
    }
}
