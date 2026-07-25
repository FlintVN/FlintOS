package flintos.midp;

import java.io.InputStream;

/**
 * Resolves MIDlet-suite resources from the application JAR.
 *
 * <p>This is a FlintOS implementation class, not a MIDP public API.
 * Uses {@link Class#getResourceAsStream(String)} which is supported
 * by FlintJVM's ZipFileReader.</p>
 */
public final class ResourceLoader {
    private static String suiteDirectory = "default";

    private ResourceLoader() {
    }

    /** Set the suite directory for RMS isolation. */
    public static synchronized void setSuiteDirectory(String directory) {
        suiteDirectory = (directory != null && directory.length() > 0) ? directory : "default";
    }

    /** Returns the current suite directory identifier. */
    public static synchronized String getSuiteDirectory() {
        return suiteDirectory;
    }

    /**
     * Opens a resource from the current application JAR.
     *
     * @param name resource path (e.g. "/MANIFEST.MF", "images/hero.png")
     * @return InputStream for the resource, or null if not found
     */
    public static InputStream open(String name) {
        if(name == null) {
            throw new NullPointerException("name");
        }
        String entry = name.startsWith("/") ? name : "/" + name;
        return ResourceLoader.class.getResourceAsStream(entry);
    }
}
