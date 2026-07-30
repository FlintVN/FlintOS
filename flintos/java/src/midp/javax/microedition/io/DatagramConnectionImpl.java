package javax.microedition.io;

import java.io.IOException;
import java.net.*;

class DatagramConnectionImpl implements UDPDatagramConnection {
    private final DatagramSocket socket;
    private final InetAddress fixedHost;
    private final int fixedPort;

    DatagramConnectionImpl(DatagramSocket socket, String host, int port) throws IOException {
        this.socket = socket;
        this.fixedHost = (host != null) ? InetAddress.getByName(host) : null;
        this.fixedPort = port;
    }

    public String getLocalAddress() throws IOException {
        return socket.getLocalAddress().getHostAddress();
    }

    public int getLocalPort() throws IOException {
        return socket.getLocalPort();
    }

    public Datagram newDatagram(int size) throws IOException {
        return new DatagramImpl(new byte[size], size);
    }

    public Datagram newDatagram(int size, String addr) throws IOException {
        Datagram d = newDatagram(size);
        d.setAddress(addr);
        return d;
    }

    public Datagram newDatagram(byte[] buf, int size) throws IOException {
        return new DatagramImpl(buf, size);
    }

    public Datagram newDatagram(byte[] buf, int size, String addr) throws IOException {
        Datagram d = newDatagram(buf, size);
        d.setAddress(addr);
        return d;
    }

    public void send(Datagram dgram) throws IOException {
        DatagramImpl impl = (DatagramImpl)dgram;

        InetAddress addr = impl.getInetAddress();
        int port = impl.getPortNum();

        if(addr == null) {
            if(fixedHost == null)
                throw new IOException("No destination address set for datagram");
            addr = fixedHost;
            port = fixedPort;
        }

        DatagramPacket packet = new DatagramPacket(impl.getData(), impl.getLength(), addr, port);
        socket.send(packet);
    }

    public void receive(Datagram dgram) throws IOException {
        DatagramImpl impl = (DatagramImpl)dgram;
        DatagramPacket packet = new DatagramPacket(impl.getData(), impl.getData().length);
        socket.receive(packet);
        impl.setLength(packet.getLength());
        impl.setAddress("datagram://" + packet.getAddress().getHostAddress() + ":" + packet.getPort());
    }

    public int getMaximumLength() throws IOException {
        return 65507;
    }

    public int getNominalLength() throws IOException {
        // return socket.getReceiveBufferSize();

        // TODO
        throw new UnsupportedOperationException();
    }

    public void close() throws IOException {
        socket.close();
    }
}
