package flint.util;

import java.io.FilterInputStream;
import java.io.InputStream;
import java.io.IOException;

public class Mp3InputStream extends FilterInputStream {
    protected Mp3Decoder decoder;
    protected byte[] buf;

    private boolean closed = false;
    private boolean mp3EOF = false;
    private boolean inEOF = false;

    public Mp3InputStream(InputStream in) {
        super(in);
        decoder = new Mp3Decoder();
        buf = new byte[4096];
    }

    private void ensureOpen() throws IOException {
        if (closed)
            throw new IOException("Stream closed");
    }

    private byte[] singleByteBuf = new byte[1];

    public int read() throws IOException {
        ensureOpen();
        return read(singleByteBuf, 0, 1) == -1 ? -1 : Byte.toUnsignedInt(singleByteBuf[0]);
    }

    public int read(byte[] b, int off, int len) throws IOException {
        ensureOpen();
        if (b == null)
            throw new NullPointerException();
        else if (off < 0 || len < 0 || len > b.length - off)
            throw new IndexOutOfBoundsException();
        else if (len == 0)
            return 0;
        int n;
        if (!inEOF && decoder.needsInput()) {
            n = in.read(buf);
            if (n > 0)
                decoder.setInput(buf, 0, n);
            else
                inEOF = true;
        }
        n = decoder.decode(b, off, len);
        if (n == 0 && inEOF && decoder.needsInput()) {
            mp3EOF = true;
            return -1;
        }
        return n;
    }

    public int available() throws IOException {
        ensureOpen();
        if (mp3EOF)
            return 0;
        else
            return 1;
    }

    private byte[] b = new byte[512];

    public long skip(long n) throws IOException {
        if (n < 0)
            throw new IllegalArgumentException("negative skip length");
        ensureOpen();
        int max = (int)Math.min(n, Integer.MAX_VALUE);
        int total = 0;
        while (total < max) {
            int len = max - total;
            if (len > b.length)
                len = b.length;
            len = read(b, 0, len);
            if (len == -1)
                break;
            total += len;
        }
        return total;
    }

    public void close() throws IOException {
        if (!closed) {
            decoder.close();
            in.close();
            closed = true;
        }
    }

    public boolean markSupported() {
        return false;
    }

    public synchronized void mark(int readlimit) {

    }

    public synchronized void reset() throws IOException {
        throw new IOException("mark/reset not supported");
    }
}
