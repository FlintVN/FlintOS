package javax.microedition.io;

import java.io.IOException;

public interface StreamConnectionNotifier extends Connection {
    public StreamConnection acceptAndOpen() throws IOException;
}
