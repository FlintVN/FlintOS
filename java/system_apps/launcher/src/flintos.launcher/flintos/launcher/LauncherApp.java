package flintos.launcher;

import flint.app.AppContext;
import flint.app.AppEvent;
import flint.app.AppInfo;
import flint.app.AppManager;
import flint.app.FlintApp;
import flint.ui.Screen;

public final class LauncherApp implements FlintApp {
    @Override
    public void onStart(AppContext context) {
        Screen.clear();
        Screen.drawText("FlintOS", 0, 0);
        AppInfo[] apps = AppManager.installedApps();
        if (apps.length == 0) {
            Screen.drawText("No apps installed", 0, 16);
            return;
        }

        for (int index = 0; index < apps.length; index++) {
            Screen.drawText(apps[index].name(), 0, 16 + index * 12);
        }
    }

    @Override
    public void onEvent(AppEvent event) {
        if ("launch:first".equals(event.type())) {
            AppInfo[] apps = AppManager.installedApps();
            if (apps.length > 0) {
                AppManager.launch(apps[0].id());
            }
        }
    }

    @Override
    public void onStop() {
    }
}
