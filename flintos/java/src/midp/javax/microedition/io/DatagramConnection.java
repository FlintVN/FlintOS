package javax.microedition.io;

import java.io.IOException;

public interface DatagramConnection extends Connection {
    public int getMaximumLength() throws IOException;

    public int getNominalLength() throws IOException;

    public void send(Datagram dgram) throws IOException;

    public void receive(Datagram dgram) throws IOException;

    public Datagram newDatagram(int size) throws IOException;

    public Datagram newDatagram(int size, String addr) throws IOException;

    public Datagram newDatagram(byte[] buf, int size) throws IOException;

    public Datagram newDatagram(byte[] buf, int size, String addr) throws IOException;
}
