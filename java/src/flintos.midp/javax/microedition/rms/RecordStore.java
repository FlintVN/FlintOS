package javax.microedition.rms;

import flintos.midp.ResourceLoader;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.EOFException;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.Hashtable;
import java.util.Vector;

/** Persistent record storage for a MIDlet suite. */
public class RecordStore {
    private static final int MAGIC = 0x46524D53;
    private static final int FORMAT_VERSION = 1;
    private static final int MAX_STORE_SIZE = 1024 * 1024;
    private static final String DATA_ROOT = "/data/rms";
    private static final Hashtable<String, RecordStore> stores = new Hashtable<>();

    private final String name;
    private final File file;
    private final Vector<byte[]> records = new Vector<>();

    private long lastModified;
    private int version;
    private boolean open;

    private RecordStore(String name, File file) throws RecordStoreException {
        this.name = name;
        this.file = file;
        this.lastModified = System.currentTimeMillis();
        this.version = 1;
        this.open = true;
        load();
    }

    public static synchronized RecordStore openRecordStore(
            String recordStoreName, boolean createIfNecessary)
            throws RecordStoreException {
        validateName(recordStoreName);
        String key = ResourceLoader.getSuiteDirectory() + "/" + recordStoreName;
        RecordStore store = stores.get(key);
        if(store != null) {
            store.open = true;
            return store;
        }

        File directory = suiteDirectory();
        File file = new File(directory, recordStoreName + ".rms");
        if(!file.exists() && !createIfNecessary)
            throw new RecordStoreException("Record store not found: " + recordStoreName);
        if(!directory.exists() && !directory.mkdirs())
            throw new RecordStoreException("Cannot create RMS directory: " + directory);

        store = new RecordStore(recordStoreName, file);
        if(!file.exists())
            store.persist();
        stores.put(key, store);
        return store;
    }

    public static synchronized void deleteRecordStore(String recordStoreName)
            throws RecordStoreException {
        validateName(recordStoreName);
        String key = ResourceLoader.getSuiteDirectory() + "/" + recordStoreName;
        RecordStore store = stores.remove(key);
        File file = new File(suiteDirectory(), recordStoreName + ".rms");
        if(store != null && store.open)
            throw new RecordStoreException("Record store is open: " + recordStoreName);
        if(!file.exists() || !file.delete())
            throw new RecordStoreException("Record store not found: " + recordStoreName);
    }

    public synchronized void closeRecordStore() throws RecordStoreException {
        ensureOpen();
        open = false;
    }

    public synchronized int addRecord(byte[] data, int offset, int numBytes)
            throws RecordStoreException {
        ensureOpen();
        byte[] record = copyRecord(data, offset, numBytes);
        ensureCapacity(sizeAfter(record.length));
        records.addElement(record);
        changed();
        return records.size();
    }

    public synchronized byte[] getRecord(int recordId) throws RecordStoreException {
        byte[] record = getRecordInternal(recordId);
        byte[] copy = new byte[record.length];
        System.arraycopy(record, 0, copy, 0, record.length);
        return copy;
    }

    public synchronized int getRecord(
            int recordId, byte[] buffer, int offset) throws RecordStoreException {
        if(buffer == null)
            throw new NullPointerException();
        byte[] record = getRecordInternal(recordId);
        if(offset < 0 || offset > buffer.length - record.length)
            throw new ArrayIndexOutOfBoundsException();
        System.arraycopy(record, 0, buffer, offset, record.length);
        return record.length;
    }

    public synchronized void setRecord(
            int recordId, byte[] newData, int offset, int numBytes)
            throws RecordStoreException {
        byte[] previous = getRecordInternal(recordId);
        byte[] record = copyRecord(newData, offset, numBytes);
        ensureCapacity(getSize() - previous.length + record.length);
        records.setElementAt(record, recordId - 1);
        changed();
    }

    public synchronized void deleteRecord(int recordId) throws RecordStoreException {
        getRecordInternal(recordId);
        records.setElementAt(null, recordId - 1);
        changed();
    }

    public synchronized int getRecordSize(int recordId) throws RecordStoreException {
        return getRecordInternal(recordId).length;
    }

    public synchronized int getNumRecords() throws RecordStoreException {
        ensureOpen();
        int count = 0;
        for(int i = 0; i < records.size(); i++) {
            if(records.elementAt(i) != null)
                count++;
        }
        return count;
    }

    public synchronized int getNextRecordID() throws RecordStoreException {
        ensureOpen();
        return records.size() + 1;
    }

    public synchronized long getLastModified() throws RecordStoreException {
        ensureOpen();
        return lastModified;
    }

    public synchronized int getSize() {
        int size = 0;
        for(int i = 0; i < records.size(); i++) {
            byte[] record = records.elementAt(i);
            if(record != null)
                size += record.length;
        }
        return size;
    }

    public synchronized int getSizeAvailable() {
        return Math.max(0, MAX_STORE_SIZE - getSize());
    }

    public String getName() {
        return name;
    }

    public synchronized int getVersion() {
        return version;
    }

    private static File suiteDirectory() throws RecordStoreException {
        String suite = sanitizePathPart(ResourceLoader.getSuiteDirectory());
        return new File(DATA_ROOT + "/" + suite);
    }

    private static void validateName(String name) {
        if(name == null)
            throw new NullPointerException();
        if(name.length() == 0 || name.length() > 32)
            throw new IllegalArgumentException("Invalid record store name");
        sanitizePathPart(name);
    }

    private static String sanitizePathPart(String value) {
        for(int i = 0; i < value.length(); i++) {
            char character = value.charAt(i);
            boolean valid = character >= 'a' && character <= 'z';
            valid |= character >= 'A' && character <= 'Z';
            valid |= character >= '0' && character <= '9';
            valid |= character == '-' || character == '_' || character == '.';
            if(!valid)
                throw new IllegalArgumentException("Invalid path character: " + character);
        }
        return value;
    }

    private static byte[] copyRecord(byte[] data, int offset, int length) {
        if(data == null && length != 0)
            throw new NullPointerException();
        if(offset < 0 || length < 0 || (data != null && offset > data.length - length))
            throw new ArrayIndexOutOfBoundsException();
        byte[] copy = new byte[length];
        if(length > 0)
            System.arraycopy(data, offset, copy, 0, length);
        return copy;
    }

    private synchronized void load() throws RecordStoreException {
        if(!file.exists())
            return;
        try (DataInputStream input = new DataInputStream(new FileInputStream(file))) {
            if(input.readInt() != MAGIC || input.readInt() != FORMAT_VERSION)
                throw new RecordStoreException("Unsupported RMS format: " + name);
            version = input.readInt();
            lastModified = input.readLong();
            int slots = input.readInt();
            if(slots < 0)
                throw new RecordStoreException("Invalid RMS record count: " + name);
            for(int i = 0; i < slots; i++) {
                int length = input.readInt();
                if(length < 0)
                    records.addElement(null);
                else {
                    byte[] record = new byte[length];
                    input.readFully(record);
                    records.addElement(record);
                }
            }
        }
        catch(EOFException exception) {
            throw new RecordStoreException("Truncated RMS file: " + name);
        }
        catch(IOException exception) {
            throw new RecordStoreException(exception.getMessage());
        }
    }

    private synchronized void persist() throws RecordStoreException {
        File temporary = new File(file.getPath() + ".tmp");
        try (DataOutputStream output = new DataOutputStream(new FileOutputStream(temporary))) {
            output.writeInt(MAGIC);
            output.writeInt(FORMAT_VERSION);
            output.writeInt(version);
            output.writeLong(lastModified);
            output.writeInt(records.size());
            for(int i = 0; i < records.size(); i++) {
                byte[] record = records.elementAt(i);
                if(record == null)
                    output.writeInt(-1);
                else {
                    output.writeInt(record.length);
                    output.write(record);
                }
            }
        }
        catch(IOException exception) {
            temporary.delete();
            throw new RecordStoreException(exception.getMessage());
        }

        if(file.exists() && !file.delete()) {
            temporary.delete();
            throw new RecordStoreException("Cannot replace RMS file: " + name);
        }
        if(!temporary.renameTo(file)) {
            temporary.delete();
            throw new RecordStoreException("Cannot commit RMS file: " + name);
        }
    }

    private void changed() throws RecordStoreException {
        version++;
        lastModified = System.currentTimeMillis();
        persist();
    }

    private void ensureCapacity(int size) throws RecordStoreFullException {
        if(size > MAX_STORE_SIZE)
            throw new RecordStoreFullException(name);
    }

    private int sizeAfter(int addedLength) {
        return getSize() + addedLength;
    }

    private void ensureOpen() throws RecordStoreNotOpenException {
        if(!open)
            throw new RecordStoreNotOpenException(name);
    }

    private byte[] getRecordInternal(int recordId) throws RecordStoreException {
        ensureOpen();
        if(recordId < 1 || recordId > records.size())
            throw new InvalidRecordIDException("id=" + recordId);
        byte[] record = records.elementAt(recordId - 1);
        if(record == null)
            throw new InvalidRecordIDException("deleted id=" + recordId);
        return record;
    }
}
