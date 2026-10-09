package flint.system;

public class Process {
    private int handle = -1;
    private String name;
    private String[] args;

    public Process() {

    }

    public Process(String name) {
        this.name = name;
    }

    public Process(String name, String... args) {
        this.name = name;
        this.args = args;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        if (name == null)
            throw new NullPointerException("name cannot be null");
        this.name = name;
    }

    public void setArgs(String... args) {
        this.args = args;
    }

    public native void start();

    public native void close();

    public native void foreground();

    public native static Process[] getProcesses();
}
