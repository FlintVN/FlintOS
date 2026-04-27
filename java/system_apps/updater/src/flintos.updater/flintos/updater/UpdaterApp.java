package flintos.updater;

import flint.app.AppContext;
import flint.app.AppEvent;
import flint.app.FlintApp;
import flint.os.UpdateManager;
import flint.ui.Screen;

public final class UpdaterApp implements FlintApp {
    @Override
    public void onStart(AppContext context) {
        Screen.clear();
        Screen.drawText("Updater", 0, 0);
        Screen.drawText(UpdateManager.hasPendingValidUpdate() ? "Update pending" : "System up to date", 0, 16);
    }

    @Override
    public void onEvent(AppEvent event) {
    }

    @Override
    public void onStop() {
    }
}
