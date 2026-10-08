package flint.ui;

import flint.drawing.Font;
import flint.drawing.Color;
import java.io.IOException;
import java.io.UncheckedIOException;

public abstract class Theme {
    static Theme defaultTheme = new DarkTheme();

    public static Theme getDefaultTheme() {
        return defaultTheme;
    }

    public static void setDefaultTheme(Theme theme) {
        if(theme == null)
            throw new NullPointerException("theme cannot be null");
        defaultTheme = theme;
    }

    protected Theme() {

    }

    public abstract Object get(String name);

    public abstract void set(String name, Object value);

    @SuppressWarnings("unchecked")
    public static <T> T getProperty(String name) {
        return (T)defaultTheme.get(name);
    }

    public static void setProperty(String name, Object value) {
        defaultTheme.set(name, value);
    }
}
