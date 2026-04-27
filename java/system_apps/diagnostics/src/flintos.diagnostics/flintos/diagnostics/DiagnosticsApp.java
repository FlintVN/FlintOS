package flintos.diagnostics;

import flint.app.AppContext;
import flint.app.AppEvent;
import flint.app.FlintApp;
import flint.os.SystemInfo;
import flint.ui.Screen;

public final class DiagnosticsApp implements FlintApp {
    @Override
    public void onStart(AppContext context) {
        Screen.clear();
        Screen.drawText("Diagnostics", 0, 0);
        Screen.drawText("OS: " + SystemInfo.osVersion(), 0, 16);
        Screen.drawText("Board: " + SystemInfo.boardName(), 0, 28);
    }

    @Override
    public void onEvent(AppEvent event) {
    }

    @Override
    public void onStop() {
    }
}
