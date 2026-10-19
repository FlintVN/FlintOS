package flint.util;

public class Mp3Decoder implements AutoCloseable {
    private int handle = -1;
    private byte[] buf;
    private int off, len;

    public Mp3Decoder() {
        handle = init();
    }

    public synchronized void setInput(byte[] b, int off, int len) {
        if (b == null)
            throw new NullPointerException();
        if (off < 0 || len < 0 || off > b.length - len)
            throw new ArrayIndexOutOfBoundsException();
        this.buf = b;
        this.off = off;
        this.len = len;
    }

    public boolean needsInput() {
        return len == 0;
    }

    private native static int init();

    public synchronized native int decode(byte[] b, int off, int len);

    @Override
    public native void close();
}
