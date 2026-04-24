package flint.app;

public final class AppEvent {
    private final String type;

    public AppEvent(String type) {
        this.type = type;
    }

    public String type() {
        return type;
    }
}
