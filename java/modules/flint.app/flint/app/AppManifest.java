package flint.app;

public final class AppManifest {
    private final String id;
    private final String name;
    private final String version;
    private final String type;
    private final String runtime;
    private final String mainClass;
    private final String[] permissions;
    private final String minOs;
    private final String minJdk;

    public AppManifest(String id, String name, String version, String type, String runtime, String mainClass, String[] permissions, String minOs, String minJdk) {
        this.id = id;
        this.name = name;
        this.version = version;
        this.type = type;
        this.runtime = runtime;
        this.mainClass = mainClass;
        this.permissions = permissions.clone();
        this.minOs = minOs;
        this.minJdk = minJdk;
    }

    public String id() { return id; }
    public String name() { return name; }
    public String version() { return version; }
    public String type() { return type; }
    public String runtime() { return runtime; }
    public String mainClass() { return mainClass; }
    public String[] permissions() { return permissions.clone(); }
    public String minOs() { return minOs; }
    public String minJdk() { return minJdk; }
}
