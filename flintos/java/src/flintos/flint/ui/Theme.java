package flint.ui;

import flint.drawing.Font;
import flint.drawing.Color;
import java.io.IOException;
import java.io.UncheckedIOException;

public abstract class Theme {
    static Theme defaultTheme = new DarkTheme();

    protected Font defaultFont;

    public static Theme getDefaultTheme() {
        return defaultTheme;
    }

    public static void setDefaultTheme(Theme theme) {
        if(theme == null)
            throw new NullPointerException("theme cannot be null");
        defaultTheme = theme;
    }

    public Theme() {

    }

    /* General */
    public abstract Color backgroundColor();
    public abstract Color primaryColor();
    public abstract Color secondaryColor();
    public abstract Color accentColor();
    public abstract Color thumbColor();

    /* Text */
    public abstract Color textColor();
    public abstract Color secondaryTextColor();
    public abstract Color disabledTextColor();
    public abstract Color hintTextColor();

    /* Component states */
    public abstract Color disabledColor();
    public abstract Color pressedColor();
    public abstract Color focusedColor();
    public abstract Color selectedColor();
    public abstract Color hoverColor();

    /* Borders */
    public abstract Color borderColor();

    /* Surfaces */
    public abstract Color surfaceColor();
    public abstract Color overlayColor();

    /* Shape */
    public int cornerRadius() {
        return 4;
    }

    /* Typography */
    public Font defaultFont() {
        if(defaultFont == null) {
            try {
                defaultFont = new Font("/sys/fonts/default.font");
            }
            catch(IOException e) {
                throw new UncheckedIOException(e);
            }
        }
        return defaultFont;
    }
}
