package flintos.midp;

import javax.microedition.midlet.MIDlet;
import javax.microedition.midlet.MIDletStateChangeException;

class AMS {
    public static void startApp(Class<? extends MIDlet> mainClass) throws Exception {
        MIDlet midlet = mainClass.getConstructor().newInstance();
        mainClass.getMethod("startApp").invoke(midlet);
    }
}
