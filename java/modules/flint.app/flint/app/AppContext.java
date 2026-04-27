package flint.app;

import flint.os.Logger;
import flint.os.Settings;
import flint.os.Storage;
import flint.ui.Input;
import flint.ui.Screen;

public interface AppContext {
    String appId();
    Screen screen();
    Input input();
    Settings settings();
    Storage storage();
    Logger logger();
}
