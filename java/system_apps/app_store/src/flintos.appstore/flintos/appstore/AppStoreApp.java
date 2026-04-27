package flintos.appstore;

import flint.app.AppContext;
import flint.app.AppEvent;
import flint.app.AppInfo;
import flint.app.AppManager;
import flint.app.FlintApp;
import flint.os.DownloadManager;
import flint.os.TaskManager;
import flint.ui.ListView;
import flint.ui.Screen;

public final class AppStoreApp implements FlintApp {
    private static final String CATALOG_URL = "https://apps.flintos.local/catalog.json";

    @Override
    public void onStart(AppContext context) {
        Screen.clear();
        Screen.drawText("App Store", 0, 0);
        AppInfo[] apps = AppManager.installedApps();
        String[] rows = new String[Math.max(apps.length + 2, 2)];
        rows[0] = "Installed: " + apps.length;
        rows[1] = "Catalog: pending sync";
        for (int index = 0; index < apps.length; index++) {
            rows[index + 2] = apps[index].name();
        }
        new ListView(rows, 0, 16).render();
    }

    @Override
    public void onEvent(AppEvent event) {
        if ("catalog:refresh".equals(event.type())) {
            TaskManager.createTask("appstore.catalog.refresh");
            DownloadManager.downloadText(CATALOG_URL, 10000);
        }
    }

    @Override
    public void onStop() {
    }
}
