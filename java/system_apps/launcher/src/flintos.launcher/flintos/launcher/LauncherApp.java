package flintos.launcher;

import flint.app.AppContext;
import flint.app.AppEvent;
import flint.app.FlintApp;
import flint.ui.Screen;

public final class LauncherApp implements FlintApp {
    @Override
    public void onStart(AppContext context) {
        Screen.clear();
        Screen.drawText("FlintOS", 0, 0);
        Screen.drawText("Launcher starting...", 0, 16);
    }

    @Override
    public void onEvent(AppEvent event) {
        // TODO: handle input/app registry events.
    }

    @Override
    public void onStop() {
        // TODO: release launcher resources.
    }
}
