package flintos.settings;

import flint.app.AppContext;
import flint.app.AppEvent;
import flint.app.FlintApp;
import flint.os.Settings;
import flint.ui.Screen;

public final class SettingsApp implements FlintApp {
    @Override
    public void onStart(AppContext context) {
        Screen.clear();
        Screen.drawText("Settings", 0, 0);
        Screen.drawText("WiFi: " + Settings.getString("wifi.ssid", "not configured"), 0, 16);
    }

    @Override
    public void onEvent(AppEvent event) {
    }

    @Override
    public void onStop() {
    }
}
