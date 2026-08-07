package com.sun.midp.lcdui;

import com.sun.midp.events.EventTypes;
import com.sun.midp.events.Event;
import com.sun.midp.events.EventListener;
import com.sun.midp.events.EventQueue;

import javax.microedition.lcdui.Canvas;
import javax.microedition.lcdui.Display;

public class RepaintEventProducer implements EventListener {
    private EventQueue eventQueue;
    private RepaintEvent pooledEvent1;
    private RepaintEvent pooledEvent2;
    private RepaintEvent pooledEvent3;
    private RepaintEvent queuedEvent;
    private RepaintEvent eventInProcess;

    public RepaintEventProducer(EventQueue theEventQueue) {
        eventQueue = theEventQueue;

        pooledEvent1 = RepaintEvent.createRepaintEvent(0, 0, 0, 0, null);
        pooledEvent2 = RepaintEvent.createRepaintEvent(0, 0, 0, 0, null);
        pooledEvent3 = RepaintEvent.createRepaintEvent(0, 0, 0, 0, null);

        eventQueue.registerEventListener(EventTypes.REPAINT_EVENT, this);
    }

    public void scheduleRepaint(int x, int y, int w, int h, Canvas target) {
        synchronized(this) {
            RepaintEvent freeEvent = pooledEvent1;
            freeEvent.setRepaintFields(x, y, w, h, target);

            if(queuedEvent == null) {
                pooledEvent1 = pooledEvent2;
                pooledEvent2 = pooledEvent3;
                pooledEvent3 = freeEvent;
                queuedEvent = freeEvent;

                eventQueue.post(queuedEvent);
            }
            else {
                if(queuedEvent.paintX1 > freeEvent.paintX1)
                    queuedEvent.paintX1 = freeEvent.paintX1;
                if(queuedEvent.paintY1 > freeEvent.paintY1)
                    queuedEvent.paintY1 = freeEvent.paintY1;
                if(queuedEvent.paintX2 < freeEvent.paintX2)
                    queuedEvent.paintX2 = freeEvent.paintX2;
                if(queuedEvent.paintY2 < freeEvent.paintY2)
                    queuedEvent.paintY2 = freeEvent.paintY2;
            }
        }
    }

    public boolean preprocess(Event event, Event waitingEvent) {
        return true;
    }

    public void process(Event genericEvent) {
        RepaintEvent event = (RepaintEvent)genericEvent;

        synchronized(this) {
            queuedEvent = null;
            eventInProcess = event;
        }

        try {
            Display.handleRepaintEvent(event.paintX1, event.paintY1, event.paintX2, event.paintY2, event.paintTarget);
        }
        finally {
            event.setRepaintFields(0, 0, 0, 0, null);

            synchronized(this) {
                eventInProcess.perUseID++;
                eventInProcess = null;
                notifyAll();
            }
        }
    }

    public void serviceRepaints() {
        if(EventQueue.isDispatchThread()) {
            if(eventInProcess != null)
                return;

            Event event = eventQueue.remove(EventTypes.REPAINT_EVENT);
            if(event != null)
                process(event);
        }
        else
            waitForCurrentRepaintEvents();
    }

    private void waitForCurrentRepaintEvents() {
        RepaintEvent eventToWaitFor = null;

        synchronized(this) {
            if(queuedEvent != null)
                eventToWaitFor = queuedEvent;
            else if(eventInProcess != null)
                eventToWaitFor = eventInProcess;
            else
                return;

            int currentEventUseID = eventToWaitFor.perUseID;
            while(eventToWaitFor.perUseID == currentEventUseID) {
                try {
                    wait();
                }
                catch(InterruptedException ie) {
                    break;
                }
            }
        }
    }
}
