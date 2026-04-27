package flintos.filemanager;

import flint.app.AppContext;
import flint.app.AppEvent;
import flint.app.FlintApp;
import flint.os.Storage;
import flint.ui.Screen;

public final class FileManagerApp implements FlintApp {
    @Override
    public void onStart(AppContext context) {
        Screen.clear();
        Screen.drawText("File Manager", 0, 0);
        Screen.drawText("Root: " + Storage.rootPath(), 0, 16);
    }

    @Override
    public void onEvent(AppEvent event) {
    }

    @Override
    public void onStop() {
    }
}
