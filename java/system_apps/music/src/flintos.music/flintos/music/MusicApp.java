package flintos.music;

import flint.app.AppContext;
import flint.app.AppEvent;
import flint.app.FlintApp;
import flint.ui.Screen;

public final class MusicApp implements FlintApp {
    public static void main(String[] args) {
        new MusicApp().onStart(null);
    }

    @Override
    public void onStart(AppContext context) {
        Screen.clear();
        Screen.drawText("Music Player", 0, 0);
        Screen.drawText("Press BOOT0", 0, 16);
        Screen.drawText("to play oii.ogg", 0, 32);
    }

    @Override
    public void onEvent(AppEvent event) {
    }

    @Override
    public void onStop() {
    }
}
