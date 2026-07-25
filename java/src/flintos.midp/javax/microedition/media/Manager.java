package javax.microedition.media;

import flintos.midp.ResourceLoader;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;

/** Entry point for creating and querying media players. */
public final class Manager {
    public static final String TONE_DEVICE_LOCATOR = "device://tone";

    private static final String[] AUDIO_CONTENT_TYPES = {
        "audio/wav", "audio/x-wav"
    };
    private static final String[] RESOURCE_PROTOCOLS = {
        "file", "resource"
    };
    private static final TimeBase systemTimeBase = new TimeBase() {
        public long getTime() {
            return System.nanoTime() / 1000L;
        }
    };

    private Manager() {
    }

    public static Player createPlayer(InputStream stream, String type)
            throws IOException, MediaException {
        if(stream == null || type == null)
            throw new IllegalArgumentException();
        if(isWaveType(type))
            return new WavePlayer(stream, type);
        throw new MediaException("Unsupported content type: " + type);
    }

    public static Player createPlayer(String locator)
            throws IOException, MediaException {
        if(locator == null)
            throw new IllegalArgumentException();
        if(TONE_DEVICE_LOCATOR.equals(locator))
            return new TonePlayer();
        if(locator.startsWith("resource://")) {
            String name = locator.substring("resource://".length());
            return new WavePlayer(ResourceLoader.open(name), contentType(name));
        }
        if(locator.startsWith("file://")) {
            String name = locator.substring("file://".length());
            return new WavePlayer(new FileInputStream(name), contentType(name));
        }
        throw new MediaException("Unsupported locator: " + locator);
    }

    public static void playTone(int note, int duration, int volume)
            throws MediaException {
        if(note < 0 || note > 127 || duration <= 0 || volume < 0 || volume > 100)
            throw new IllegalArgumentException();
        TonePlayer player = new TonePlayer(note, duration);
        player.getVolumeControl().setLevel(volume);
        player.start();
    }

    public static String[] getSupportedContentTypes(String protocol) {
        if(protocol == null || supportsProtocol(protocol))
            return AUDIO_CONTENT_TYPES.clone();
        return new String[0];
    }

    public static String[] getSupportedProtocols(String contentType) {
        if(contentType == null || isWaveType(contentType))
            return RESOURCE_PROTOCOLS.clone();
        return new String[0];
    }

    public static TimeBase getSystemTimeBase() {
        return systemTimeBase;
    }

    private static boolean supportsProtocol(String protocol) {
        return "file".equals(protocol) || "resource".equals(protocol);
    }

    private static boolean isWaveType(String type) {
        return "audio/wav".equalsIgnoreCase(type)
                || "audio/x-wav".equalsIgnoreCase(type);
    }

    private static String contentType(String name) throws MediaException {
        String lowerName = name.toLowerCase();
        if(lowerName.endsWith(".wav"))
            return "audio/wav";
        throw new MediaException("Unknown media type: " + name);
    }
}
