package javax.microedition.io;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.net.URL;
import java.net.HttpURLConnection;

class HttpConnectionImpl implements HttpConnection {
    private final HttpURLConnection conn;

    HttpConnectionImpl(HttpURLConnection conn) {
        this.conn = conn;
    }

    public String getType() {
        return conn.getContentType();
    }

    public String getEncoding() {
        return conn.getContentEncoding();
    }

    public long getLength() {
        return conn.getContentLengthLong();
    }

    public String getURL() {
        return conn.getURL().toString();
    }

    public String getProtocol() {
        return conn.getURL().getProtocol();
    }

    public String getHost() {
        return conn.getURL().getHost();
    }

    public String getFile() {
        return conn.getURL().getPath();
    }

    public String getRef() {
        return conn.getURL().getRef();
    }

    public String getQuery() {
        return conn.getURL().getQuery();
    }

    public int getPort() {
        URL url = conn.getURL();
        int port = url.getPort();
        return port != -1 ? port : url.getDefaultPort();
    }

    public String getRequestMethod() {
        return conn.getRequestMethod();
    }

    public void setRequestMethod(String method) throws IOException {
        if (!HttpConnection.GET.equals(method) && !HttpConnection.POST.equals(method) && !HttpConnection.HEAD.equals(method))
            throw new IOException("Unsupported method: " + method);
        conn.setRequestMethod(method);
    }

    public String getRequestProperty(String key) {
        return conn.getRequestProperty(key);
    }

    public void setRequestProperty(String key, String value) throws IOException {
        try {
            conn.setRequestProperty(key, value);
        }
        catch (IllegalStateException e) {
            throw new IOException(e.getMessage());
        }
    }

    public int getResponseCode() throws IOException {
        return conn.getResponseCode();
    }

    public String getResponseMessage() throws IOException {
        return conn.getResponseMessage();
    }

    public long getExpiration() throws IOException {
        return conn.getExpiration();
    }

    public long getDate() throws IOException {
        return conn.getDate();
    }

    public long getLastModified() throws IOException {
        return conn.getLastModified();
    }

    public String getHeaderField(String name) throws IOException {
        return conn.getHeaderField(name);
    }

    public int getHeaderFieldInt(String name, int def) throws IOException {
        return conn.getHeaderFieldInt(name, def);
    }

    public long getHeaderFieldDate(String name, long def) throws IOException {
        return conn.getHeaderFieldDate(name, def);
    }

    public String getHeaderField(int n) throws IOException {
        return conn.getHeaderField(n + 1);
    }

    public String getHeaderFieldKey(int n) throws IOException {
        return conn.getHeaderFieldKey(n + 1);
    }

    public InputStream openInputStream() throws IOException {
        return conn.getInputStream();
    }

    public DataInputStream openDataInputStream() throws IOException {
        return new DataInputStream(openInputStream());
    }

    public OutputStream openOutputStream() throws IOException {
        try {
            conn.setDoOutput(true);
        }
        catch (IllegalStateException e) {
            throw new IOException(e.getMessage());
        }
        return conn.getOutputStream();
    }

    public DataOutputStream openDataOutputStream() throws IOException {
        return new DataOutputStream(openOutputStream());
    }

    public void close() throws IOException {
        conn.disconnect();
    }
}
