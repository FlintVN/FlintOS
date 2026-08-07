package com.sun.midp.events;

public class EventQueue implements Runnable {
    private static EventQueue eventQueue;

    private Object eventQueueLock;
    Thread eventQueueThread;
    Event nextEvent;
    Event lastEvent;
    EventListener[] listeners;

    static {
        eventQueue = new EventQueue();
        eventQueue.start();
    }

    public static boolean isDispatchThread() {
        return (Thread.currentThread() == eventQueue.eventQueueThread);
    }

    public static EventQueue getEventQueue() {
        return eventQueue;
    }

    EventQueue() {
        eventQueueThread = new Thread(this);
        eventQueueLock = new Object();
        listeners = new EventListener[8];
    }

    void start() {
        eventQueueThread.start();
    }

    public void registerEventListener(int eventType, EventListener listener) {
        if(eventType <= 0)
            throw new IllegalArgumentException("Event type is not positive");
        if(listener == null)
            throw new IllegalArgumentException("Listener is null");

        synchronized(eventQueueLock) {
            if(eventType > listeners.length) {
                EventListener[] newTable = new EventListener[eventType + 1];

                for(int i = 0; i < listeners.length; i++)
                    newTable[i] = listeners[i];

                listeners = newTable;
            }
            listeners[eventType - 1] = listener;
        }
    }

    public void post(Event event) {
        synchronized(eventQueueLock) {
            if(event.type < 1)
                return;
            if(event.type > listeners.length)
                return;

            if(lastEvent != null)
                lastEvent.next = event;
            else
                nextEvent = event;

            lastEvent = event;

            eventQueueLock.notifyAll();
        }
    }

    public Event remove(int eventType) {
        Event prev = null;
        Event result = null;
        EventListener listener;

        synchronized(eventQueueLock) {
            for(Event current = nextEvent; current != null; prev = current, current = current.next) {
                if(current.type == eventType) {
                    result = current;
                    break;
                }
            }

            if(result == null)
                return null;

            listener = listeners[eventType - 1];

            if(result == nextEvent)
                nextEvent = result.next;
            if(result == lastEvent)
                lastEvent = prev;
            if(prev != null)
                prev.next = result.next;

            result.next = null;
        }

        return result;
    }

    public void run() {
        Event event = null;
        EventListener listener;

        for(;;) {
            try {
                synchronized(eventQueueLock) {
                    while(nextEvent == null)
                        eventQueueLock.wait();

                    event = nextEvent;
                    nextEvent = event.next;
                    event.next = null;
                    if(nextEvent == null)
                        lastEvent = null;

                    if(event.type == EventTypes.EVENT_QUEUE_SHUTDOWN) {
                        eventQueueLock.notifyAll();
                        return;
                    }

                    listener = listeners[event.type - 1];
                }

                listener.process(event);

                event = null;
            }
            catch(Throwable t) {
                if(!(t instanceof OutOfMemoryError))
                    t.printStackTrace();
            }
        }
    }
}
