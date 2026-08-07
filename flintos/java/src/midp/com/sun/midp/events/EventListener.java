package com.sun.midp.events;

public interface EventListener {
    public boolean preprocess(Event event, Event waitingEvent);

    public void process(Event event);
}
