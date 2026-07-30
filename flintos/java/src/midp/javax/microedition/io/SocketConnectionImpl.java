package javax.microedition.io;

import java.io.*;
import java.net.Socket;

class SocketConnectionImpl implements SocketConnection {
    private final Socket socket;

    SocketConnectionImpl(Socket socket) {
        this.socket = socket;
    }

    public void close() throws IOException {
        socket.close();
    }

    public DataInputStream openDataInputStream() throws IOException {
        return new DataInputStream(socket.getInputStream());
    }

    public InputStream openInputStream() throws IOException {
        return socket.getInputStream();
    }

    public DataOutputStream openDataOutputStream() throws IOException {
        return new DataOutputStream(socket.getOutputStream());
    }

    public OutputStream openOutputStream() throws IOException {
        return socket.getOutputStream();
    }

    public String getAddress() throws IOException {
        return socket.getInetAddress().getHostAddress();
    }

    public String getLocalAddress() throws IOException {
        return socket.getLocalAddress().getHostAddress();
    }

    public int getPort() throws IOException {
        return socket.getPort();
    }

    public int getLocalPort() throws IOException {
        return socket.getLocalPort();
    }

    public void setSocketOption(byte option, int value) throws IOException {
        switch(option) {
            case DELAY: socket.setTcpNoDelay(value == 0); break;
            case LINGER: socket.setSoLinger(value > 0, value); break;
            // TODO
            // case KEEPALIVE: socket.setKeepAlive(value != 0); break;
            // case RCVBUF: socket.setReceiveBufferSize(value); break;
            // case SNDBUF: socket.setSendBufferSize(value); break;
            default: throw new IllegalArgumentException();
        }
    }

    public int getSocketOption(byte option) throws IOException {
        switch(option) {
            case DELAY: return socket.getTcpNoDelay() ? 0 : 1;
            case LINGER: return Math.max(socket.getSoLinger(), 0);
            // TODO
            // case KEEPALIVE: return socket.getKeepAlive() ? 1 : 0;
            // case RCVBUF: return socket.getReceiveBufferSize();
            // case SNDBUF: return socket.getSendBufferSize();
            default: throw new IllegalArgumentException();
        }
    }
}
