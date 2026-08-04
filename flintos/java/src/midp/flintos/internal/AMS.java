package flintos.internal;

import javax.microedition.midlet.MIDlet;
import javax.microedition.midlet.MIDletStateChangeException;

class AMS {
    public static void startApp(Class<? extends MIDlet> mainClass) throws Exception {
        System.out.println("Start" + mainClass.getName());
        MIDlet midlet = mainClass.getConstructor().newInstance();
        mainClass.getMethod("startApp").invoke(midlet);
    }
}
