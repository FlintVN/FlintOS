package flint.app;

public interface FlintApp {
    void onStart(AppContext context);
    void onEvent(AppEvent event);
    void onStop();
}
