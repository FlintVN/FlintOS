package javax.microedition.io;

import java.io.*;

public interface Datagram extends DataInput, DataOutput {
    public String getAddress();

    public byte[] getData();

    public int getLength();

    public int getOffset();

    public void setAddress(String addr) throws IOException;

    public void setAddress(Datagram reference);

    public void setLength(int len);

    public void setData(byte[] buffer, int offset, int len);

    public void reset();
}
