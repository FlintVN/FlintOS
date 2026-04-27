package flint.app;

public final class AppInfo {
    private final String id;
    private final String name;
    private final String mainClass;
    private final String version;
    private final String type;
    private final String runtime;

    public AppInfo(String id, String name, String mainClass) {
        this(id, name, mainClass, "", "", "");
    }

    public AppInfo(String id, String name, String mainClass, String version, String type, String runtime) {
        this.id = id;
        this.name = name;
        this.mainClass = mainClass;
        this.version = version;
        this.type = type;
        this.runtime = runtime;
    }

    public String id() {
        return id;
    }

    public String name() {
        return name;
    }

    public String mainClass() {
        return mainClass;
    }

    public String version() {
        return version;
    }

    public String type() {
        return type;
    }

    public String runtime() {
        return runtime;
    }
}
