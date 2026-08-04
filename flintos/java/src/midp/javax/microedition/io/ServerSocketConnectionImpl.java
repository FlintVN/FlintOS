package javax.microedition.io;

import java.io.IOException;
import java.net.ServerSocket;

class ServerSocketConnectionImpl implements ServerSocketConnection {
    private final ServerSocket server;

    ServerSocketConnectionImpl(ServerSocket server) {
        this.server = server;
    }

    public StreamConnection acceptAndOpen() throws IOException {
        return new SocketConnectionImpl(server.accept());
    }

    public String getLocalAddress() throws IOException {
        return server.getInetAddress().getHostAddress();
    }

    public int getLocalPort() throws IOException {
        return server.getLocalPort();
    }

    public void close() throws IOException {
        server.close();
    }
}
