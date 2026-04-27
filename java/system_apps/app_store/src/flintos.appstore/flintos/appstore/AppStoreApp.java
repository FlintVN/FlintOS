package flintos.appstore;

import flint.app.AppContext;
import flint.app.AppEvent;
import flint.app.AppInfo;
import flint.app.AppManager;
import flint.app.FlintApp;
import flint.ui.Screen;

public final class AppStoreApp implements FlintApp {
    @Override
    public void onStart(AppContext context) {
        Screen.clear();
        Screen.drawText("App Store", 0, 0);
        Screen.drawText("Installed: " + AppManager.installedApps().length, 0, 16);
    }

    @Override
    public void onEvent(AppEvent event) {
    }

    @Override
    public void onStop() {
    }
}
