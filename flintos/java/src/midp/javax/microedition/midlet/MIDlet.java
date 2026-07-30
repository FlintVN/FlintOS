package javax.microedition.midlet;

import java.util.Hashtable;
import javax.microedition.io.ConnectionNotFoundException;

public abstract class MIDlet {
    private static java.util.Hashtable<String, String> props;

    protected MIDlet() {

    }

    protected abstract void startApp() throws MIDletStateChangeException;

    protected abstract void pauseApp();

    protected abstract void destroyApp(boolean unconditional) throws MIDletStateChangeException;

    public final void notifyDestroyed() {
        // TODO
    }

    public final void notifyPaused() {
        // TODO
    }

    public final String getAppProperty(String key) {
        if(props == null) {
            synchronized(this) {
                if(props == null)
                    props = loadProps();
            }
        }
        return props.get(key);
    }

    private Hashtable<String, String> loadProps() {
        Hashtable<String, String> p = new Hashtable<>();
        try(java.io.InputStream is = this.getClass().getResourceAsStream("/MANIFEST.MF")) {
            if(is != null) {
                String s = new String(is.readAllBytes());
                int start = 0;
                while(start < s.length()) {
                    int nl = s.indexOf('\n', start);
                    if(nl < 0)
                        nl = s.length();
                    String line = s.substring(start, nl);
                    int colon = line.indexOf(':');
                    if(colon > 0)
                        p.put(line.substring(0, colon).trim(), line.substring(colon + 1).trim());
                    start = nl + 1;
                }
            }
        }
        catch(Exception e) {

        }
        return p;
    }

    public final void resumeRequest() {
        // TODO
    }

    public final boolean platformRequest(String URL) throws ConnectionNotFoundException {
        // TODO
        return false;
    }

    public final int checkPermission(String permission) {
        return -1;
    }
}
