package javax.microedition.media;

import java.io.IOException;

public interface Player extends Controllable {
    static final int UNREALIZED = 100;

    static final int REALIZED = 200;

    static final int PREFETCHED = 300;

    static final int STARTED = 400;

    static final int CLOSED = 0;

    static final long TIME_UNKNOWN = -1;

    void realize() throws MediaException;

    void prefetch() throws MediaException;

    void start() throws MediaException;

    void stop() throws MediaException;

    void deallocate();

    void close();

    long setMediaTime(long now) throws MediaException;

    long getMediaTime();

    int getState();

    long getDuration();

    String getContentType();

    void setLoopCount(int count);

    void addPlayerListener(PlayerListener playerListener);

    void removePlayerListener(PlayerListener playerListener);
}
