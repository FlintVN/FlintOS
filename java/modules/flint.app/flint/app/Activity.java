package flint.app;

public abstract class Activity implements FlintApp {
    private AppContext context;
    private Intent intent;
    private boolean foreground;
    private AppResult result;

    public final void attach(AppContext context, Intent intent) {
        this.context = context;
        this.intent = intent;
    }

    @Override
    public final void onStart(AppContext context) {
        attach(context, new Intent());
        onCreate();
        onStart();
        onResume();
    }

    @Override
    public void onEvent(AppEvent event) {}

    @Override
    public final void onStop() {
        onPause();
        onStopActivity();
        onDestroy();
    }

    protected void onCreate() {}
    protected void onStart() {}

    protected void onResume() {
        foreground = true;
    }

    protected void onPause() {
        foreground = false;
    }

    protected void onStopActivity() {}
    protected void onDestroy() {}

    public final AppContext context() {
        return context;
    }

    public final Intent getIntent() {
        return intent;
    }

    public final boolean hasForeground() {
        return foreground;
    }

    public final void ifForeground(Runnable runnable) {
        if (foreground) {
            runnable.run();
        }
    }

    public final boolean startActivity(Intent intent) {
        return AppManager.startActivity(intent);
    }

    public final boolean startActivityForResult(Intent intent) {
        return AppManager.startActivityForResult(intent);
    }

    public final void setResult(String resultCode, String data) {
        result = new AppResult(resultCode, data);
    }

    public final AppResult result() {
        return result;
    }

    public final void finish() {
        AppManager.finishActivity();
    }
}
