package flintos.launcher;

import flint.app.AppContext;
import flint.app.AppEvent;
import flint.app.AppInfo;
import flint.app.AppManager;
import flint.app.FlintApp;
import flint.app.Intent;
import flint.ui.ListView;
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

        String[] names = new String[apps.length];
        for (int index = 0; index < apps.length; index++) {
            names[index] = apps[index].name();
        }
        new ListView(names, 0, 16).render();
    }

    @Override
    public void onEvent(AppEvent event) {
        if ("launch:first".equals(event.type())) {
            AppInfo[] apps = AppManager.installedApps();
            if (apps.length > 0) {
                AppManager.startActivity(Intent.explicit(apps[0].id()));
            }
        }
        if ("open:settings".equals(event.type())) {
            AppManager.startActivity(Intent.action(Intent.ACTION_SETTINGS));
        }
        if ("open:appstore".equals(event.type())) {
            AppManager.startActivity(Intent.action(Intent.ACTION_APP_STORE));
        }
    }

    @Override
    public void onStop() {
    }
}
