package javax.microedition.io;

import java.io.*;
import java.net.*;

public class Connector {
    public final static int READ = 1;
    public final static int WRITE = 2;
    public final static int READ_WRITE = (READ | WRITE);

    private Connector() {

    }

    public static Connection open(String name) throws IOException {
        return open(name, READ_WRITE);
    }

    public static Connection open(String name, int mode) throws IOException {
        return open(name, mode, false);
    }

    public static Connection open(String name, int mode, boolean timeouts) throws IOException {
        try {
            return openPrim(name, mode, timeouts);
        }
        catch(ClassNotFoundException x) {

        }

        throw new ConnectionNotFoundException();
    }

    private static Connection openPrim(String name, int mode, boolean timeouts) throws IOException, ClassNotFoundException {
        if(mode != READ && mode != WRITE && mode != READ_WRITE)
            throw new IllegalArgumentException();

        if(name == null)
            throw new IllegalArgumentException();

        int colon = name.indexOf(':');

        if(colon < 1)
            throw new IllegalArgumentException();

        String protocol = name.substring(0, colon);
        char[] chars = protocol.toCharArray();
        for(int i = 0; i < chars.length; ++i) {
            char c = chars[i];
            if(('A' <= c && c <= 'Z') || ('a' <= c && c <= 'z') || ((i > 0) && (('0' <= c && c <= '9') || c == '+' || c == '-' || c == '.')))
                continue;
            throw new IllegalArgumentException("Invalid protocol name");
        }

        String target = name.substring(colon + 1);
        switch(protocol) {
            case "socket":
                return openSocket(target);
            case "serversocket":
                return openServerSocket(target);
            case "datagram":
                return openDatagram(target);
            default:
                throw new ConnectionNotFoundException("Unsupported protocol: " + protocol);
        }
    }

    private static String[] parseHostPort(String target, boolean portRequired) {
        if(!target.startsWith("//"))
            throw new IllegalArgumentException("Malformed connection string: " + target);

        String hostPort = target.substring(2);

        int semi = hostPort.indexOf(';');
        if(semi >= 0)
            hostPort = hostPort.substring(0, semi);

        int colon = hostPort.lastIndexOf(':');
        String host;
        String port;

        if(colon < 0) {
            if(portRequired)
                throw new IllegalArgumentException("Missing port in: " + target);
            host = hostPort;
            port = null;
        }
        else {
            host = hostPort.substring(0, colon);
            port = hostPort.substring(colon + 1);
        }

        if(host.isEmpty())
            host = null;

        return new String[] { host, port };
    }

    private static int parsePort(String portStr) {
        try {
            int port = Integer.parseInt(portStr);
            if(port < 0 || port > 65535)
                throw new IllegalArgumentException("Port out of range: " + port);
            return port;
        }
        catch(NumberFormatException e) {
            throw new IllegalArgumentException("Invalid port: " + portStr);
        }
    }

    private static Connection openSocket(String target) throws IOException {
        String[] hp = parseHostPort(target, true);
        if(hp[0] == null)
            throw new IllegalArgumentException("socket:// requires a host");

        Socket socket = new Socket(hp[0], parsePort(hp[1]));
        return new SocketConnectionImpl(socket);
    }

    private static Connection openServerSocket(String target) throws IOException {
        String[] hp = parseHostPort(target, true);
        ServerSocket server = new ServerSocket(parsePort(hp[1]));
        return new ServerSocketConnectionImpl(server);
    }

    private static Connection openDatagram(String target) throws IOException {
        String[] hp = parseHostPort(target, false);
        String host = hp[0];
        int port = (hp[1] != null) ? parsePort(hp[1]) : 0;

        if(host == null) {
            DatagramSocket socket = new DatagramSocket(port);
            return new DatagramConnectionImpl(socket, null, 0);
        }
        else {
            DatagramSocket socket = new DatagramSocket();
            return new DatagramConnectionImpl(socket, host, port);
        }
    }

    public static DataInputStream openDataInputStream(String name) throws IOException {
        InputConnection con = null;
        try {
            con = (InputConnection)Connector.open(name, Connector.READ);
        }
        catch(ClassCastException e) {
            throw new IOException();
        }

        try {
            return con.openDataInputStream();
        }
        finally {
            con.close();
        }
    }

    public static DataOutputStream openDataOutputStream(String name) throws IOException {
        OutputConnection con = null;
        try {
            con = (OutputConnection)Connector.open(name, Connector.WRITE);
        }
        catch(ClassCastException e) {
            throw new IOException();
        }

        try {
            return con.openDataOutputStream();
        }
        finally {
            con.close();
        }
    }

    public static InputStream openInputStream(String name) throws IOException {
        return openDataInputStream(name);
    }

    public static OutputStream openOutputStream(String name) throws IOException {
        return openDataOutputStream(name);
    }
}
