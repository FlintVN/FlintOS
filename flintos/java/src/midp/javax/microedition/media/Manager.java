package javax.microedition.media;

import java.io.InputStream;
import java.io.IOException;

public final class Manager {
    public final static String TONE_DEVICE_LOCATOR = "device://tone";
    // private final static String RADIO_CAPTURE_LOCATOR = "capture://radio";
    // private final static String DS_ERR = "Cannot create a DataSource for: ";
    private final static String PL_ERR = "Cannot create a Player for: ";
    private final static String REDIRECTED_MSG = " with exception message: ";

    private Manager() {

    }

    public static String[] getSupportedContentTypes(String protocol) {
        return new String[0];
    }

    public static String[] getSupportedProtocols(String content_type) {
        return new String[0];
    }

    public static Player createPlayer(String locator) throws IOException, MediaException {
        if(locator == null)
            throw new IllegalArgumentException();

        throw new MediaException("Cannot create Player");
    }

    public static Player createPlayer(InputStream stream, String type) throws IOException, MediaException {
        if(stream == null)
            throw new IllegalArgumentException();

        if(type == null)
            throw new MediaException(PL_ERR + "NULL content-type");
        
        throw new MediaException("Cannot create Player");
    }

    public static void playTone(int note, int duration, int volume) throws MediaException {
        if(note < 0 || note > 127 || duration <= 0)
            throw new IllegalArgumentException("bad param");
    }
}
