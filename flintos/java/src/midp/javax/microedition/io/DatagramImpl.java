package javax.microedition.io;

import java.io.*;
import java.net.InetAddress;
import java.net.UnknownHostException;

class DatagramImpl implements Datagram {
    private byte[] buffer;
    private int capacity;
    private int length;
    private int writePos;
    private int readPos;

    private InetAddress address;
    private int port;

    DatagramImpl(byte[] buf, int size) {
        if(buf == null)
            throw new IllegalArgumentException("buffer is null");
        if(size < 0 || size > buf.length)
            throw new IllegalArgumentException("invalid size");

        this.buffer = buf;
        this.capacity = size;
        this.length = 0;
        this.writePos = 0;
        this.readPos = 0;
        this.address = null;
        this.port = 0;
    }

    public String getAddress() {
        if(address == null)
            return null;
        return "datagram://" + address.getHostAddress() + ":" + port;
    }

    public void setAddress(String addr) {
        if(addr == null) {
            address = null;
            port = 0;
            return;
        }

        String target = addr;
        int colon2 = addr.indexOf("://");
        if(colon2 >= 0)
            target = addr.substring(colon2 + 3);

        int colon = target.lastIndexOf(':');
        if(colon < 0)
            throw new IllegalArgumentException("Missing port in address: " + addr);

        String host = target.substring(0, colon);
        String portStr = target.substring(colon + 1);

        try {
            this.address = host.isEmpty() ? InetAddress.getLocalHost() : InetAddress.getByName(host);
            this.port = Integer.parseInt(portStr);
        }
        catch(UnknownHostException e) {
            throw new IllegalArgumentException("Unknown host: " + host);
        }
        catch(NumberFormatException e) {
            throw new IllegalArgumentException("Invalid port: " + portStr);
        }
    }

    public void setAddress(Datagram reference) {
        if(!(reference instanceof DatagramImpl))
            throw new IllegalArgumentException("Unsupported Datagram implementation");

        DatagramImpl ref = (DatagramImpl)reference;
        this.address = ref.address;
        this.port = ref.port;
    }

    InetAddress getInetAddress() {
        return address;
    }

    int getPortNum() {
        return port;
    }

    public byte[] getData() {
        return buffer;
    }

    public int getLength() {
        return length;
    }

    public void setLength(int len) {
        if(len < 0 || len > capacity)
            throw new IllegalArgumentException("length out of range: " + len);
        this.length = len;
        this.readPos = 0;
    }

    public int getOffset() {
        return 0;
    }

    public void setData(byte[] buf, int offset, int len) {
        if(buf == null)
            throw new IllegalArgumentException("buffer is null");
        if(offset != 0)
            throw new IllegalArgumentException("offset must be 0");
        if(len < 0 || len > buf.length)
            throw new IllegalArgumentException("invalid length");

        this.buffer = buf;
        this.capacity = buf.length;
        this.length = len;
        this.writePos = 0;
        this.readPos = 0;
    }

    public void reset() {
        this.writePos = 0;
        this.readPos = 0;
        this.length = 0;
    }

    public void write(int b) throws IOException {
        ensureWriteSpace(1);
        buffer[writePos++] = (byte)b;
        advanceLength();
    }

    public void write(byte[] b) throws IOException {
        write(b, 0, b.length);
    }

    public void write(byte[] b, int off, int len) throws IOException {
        if(b == null)
            throw new NullPointerException();
        if(off < 0 || len < 0 || off + len > b.length)
            throw new IndexOutOfBoundsException();

        ensureWriteSpace(len);
        System.arraycopy(b, off, buffer, writePos, len);
        writePos += len;
        advanceLength();
    }

    public void writeBoolean(boolean v) throws IOException {
        write(v ? 1 : 0);
    }

    public void writeByte(int v) throws IOException {
        write(v);
    }

    public void writeShort(int v) throws IOException {
        write((v >>> 8) & 0xFF);
        write(v & 0xFF);
    }

    public void writeChar(int v) throws IOException {
        writeShort(v);
    }

    public void writeInt(int v) throws IOException {
        write((v >>> 24) & 0xFF);
        write((v >>> 16) & 0xFF);
        write((v >>> 8)  & 0xFF);
        write(v & 0xFF);
    }

    public void writeLong(long v) throws IOException {
        writeInt((int)(v >>> 32));
        writeInt((int)v);
    }

    public void writeFloat(float v) throws IOException {
        writeInt(Float.floatToIntBits(v));
    }

    public void writeDouble(double v) throws IOException {
        writeLong(Double.doubleToLongBits(v));
    }

    public void writeBytes(String s) throws IOException {
        int len = s.length();
        for(int i = 0; i < len; ++i)
            write((byte)s.charAt(i));
    }

    public void writeChars(String s) throws IOException {
        int len = s.length();
        for(int i = 0; i < len; ++i)
            writeChar(s.charAt(i));
    }

    public void writeUTF(String s) throws IOException {
        int strlen = s.length();
        int utflen = 0;

        for(int i = 0; i < strlen; ++i) {
            int c = s.charAt(i);
            if(c >= 0x0001 && c <= 0x007F) utflen += 1;
            else if(c > 0x07FF) utflen += 3;
            else utflen += 2;
        }

        if(utflen > 65535)
            throw new UTFDataFormatException("String too long for writeUTF: " + utflen);

        writeShort(utflen);

        for(int i = 0; i < strlen; ++i) {
            int c = s.charAt(i);
            if(c >= 0x0001 && c <= 0x007F) {
                write(c);
            }
            else if(c > 0x07FF) {
                write(0xE0 | ((c >> 12) & 0x0F));
                write(0x80 | ((c >> 6) & 0x3F));
                write(0x80 | (c & 0x3F));
            }
            else {
                write(0xC0 | ((c >> 6) & 0x1F));
                write(0x80 | (c & 0x3F));
            }
        }
    }

    private void ensureWriteSpace(int n) throws IOException {
        if(writePos + n > capacity)
            throw new IOException("Datagram buffer overflow");
    }

    private void advanceLength() {
        if(writePos > length)
            length = writePos;
    }

    public boolean readBoolean() throws IOException {
        return readUnsignedByte() != 0;
    }

    public byte readByte() throws IOException {
        return (byte)readUnsignedByte();
    }

    public int readUnsignedByte() throws IOException {
        if(readPos >= length)
            throw new EOFException();
        return buffer[readPos++] & 0xFF;
    }

    public short readShort() throws IOException {
        return (short)readUnsignedShort();
    }

    public int readUnsignedShort() throws IOException {
        int b1 = readUnsignedByte();
        int b2 = readUnsignedByte();
        return (b1 << 8) | b2;
    }

    public char readChar() throws IOException {
        return (char)readUnsignedShort();
    }

    public int readInt() throws IOException {
        int b1 = readUnsignedByte();
        int b2 = readUnsignedByte();
        int b3 = readUnsignedByte();
        int b4 = readUnsignedByte();
        return (b1 << 24) | (b2 << 16) | (b3 << 8) | b4;
    }

    public long readLong() throws IOException {
        long hi = readInt() & 0xFFFFFFFFL;
        long lo = readInt() & 0xFFFFFFFFL;
        return (hi << 32) | lo;
    }

    public float readFloat() throws IOException {
        return Float.intBitsToFloat(readInt());
    }

    public double readDouble() throws IOException {
        return Double.longBitsToDouble(readLong());
    }

    public void readFully(byte[] b) throws IOException {
        readFully(b, 0, b.length);
    }

    public void readFully(byte[] b, int off, int len) throws IOException {
        if(off < 0 || len < 0 || off + len > b.length)
            throw new IndexOutOfBoundsException();
        if(readPos + len > length)
            throw new EOFException();

        System.arraycopy(buffer, readPos, b, off, len);
        readPos += len;
    }

    public int skipBytes(int n) throws IOException {
        int avail = length - readPos;
        int toSkip = Math.min(avail, Math.max(n, 0));
        readPos += toSkip;
        return toSkip;
    }

    public String readUTF() throws IOException {
        int utflen = readUnsignedShort();
        StringBuilder sb = new StringBuilder(utflen);

        int endPos = readPos + utflen;
        if(endPos > length)
            throw new EOFException();

        while(readPos < endPos) {
            int c = readUnsignedByte();
            int c2, c3;

            switch(c >> 4) {
                case 0: case 1: case 2: case 3:
                case 4: case 5: case 6: case 7:
                    // 0xxxxxxx
                    sb.append((char)c);
                    break;
                case 12: case 13:
                    // 110xxxxx 10xxxxxx
                    c2 = readUnsignedByte();
                    if((c2 & 0xC0) != 0x80)
                        throw new UTFDataFormatException("Malformed UTF-8");
                    sb.append((char)(((c & 0x1F) << 6) | (c2 & 0x3F)));
                    break;
                case 14:
                    // 1110xxxx 10xxxxxx 10xxxxxx
                    c2 = readUnsignedByte();
                    c3 = readUnsignedByte();
                    if((c2 & 0xC0) != 0x80 || (c3 & 0xC0) != 0x80)
                        throw new UTFDataFormatException("Malformed UTF-8");
                    sb.append((char)(((c & 0x0F) << 12) | ((c2 & 0x3F) << 6) | (c3 & 0x3F)));
                    break;
                default:
                    throw new UTFDataFormatException("Malformed UTF-8");
            }
        }

        return sb.toString();
    }

    public String readLine() throws IOException {
        StringBuilder sb = new StringBuilder();
        boolean any = false;

        while(readPos < length) {
            any = true;
            int c = readUnsignedByte();
            if(c == '\n') break;
            if(c == '\r') {
                if(readPos < length && buffer[readPos] == '\n')
                    readPos++;
                break;
            }
            sb.append((char)c);
        }

        return any ? sb.toString() : null;
    }
}
