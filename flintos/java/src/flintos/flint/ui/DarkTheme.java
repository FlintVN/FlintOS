package flint.ui;

import flint.drawing.Font;
import flint.drawing.Color;

import java.io.IOException;
import java.io.UncheckedIOException;
import java.util.Map;
import java.util.HashMap;
import java.util.NoSuchElementException;

public class DarkTheme extends Theme {
    private Map<String, Object> values;

    public DarkTheme() {
        this.values = new HashMap<>();
    }

    @Override
    public Object get(String name) {
        synchronized(values) {
            Object ret = values.get(name);
            if(ret == null) {
                ret = getDefault(name);
                if(ret == null)
                    throw new NoSuchElementException("No value for: " + name);
                values.put(name, ret);
            }
            return ret;
        }
    }

    @Override
    public void set(String name, Object value) {
        if(value == null)
            throw new NullPointerException("The value cannot be null");
        synchronized(values) {
            values.put(name, value);
        }
    }

    private static Object getDefault(String name) {
        try {
            return switch(name) {
                case "backgroundColor" -> new Color(0xFF121212);    // #121212
                case "primaryColor" -> new Color(0xFF2196F3);       // #2196F3
                case "secondaryColor" -> new Color(0xFF9C27B0);     // #9C27B0
                case "accentColor" -> new Color(0xFF03A9F4);        // #03A9F4
                case "thumbColor" -> new Color(0xFFDCDCDC);         // #DCDCDC
                case "textColor" -> new Color(0xFFE0E0E0);          // #E0E0E0
                case "secondaryTextColor" -> new Color(0xFFAAAAAA); // #AAAAAA
                case "disabledTextColor" -> new Color(0xFF5A5A5A);  // #5A5A5A
                case "hintTextColor" -> new Color(0xFF787878);      // #787878
                case "disabledColor" -> new Color(0xFF373737);      // #373737
                case "pressedColor" -> new Color(0xFF1976BE);       // #1976BE
                case "focusedColor" -> new Color(0xFF42A5F5);       // #42A5F5
                case "selectedColor" -> new Color(0xFF2D2D2D);      // #2D2D2D
                case "hoverColor" -> new Color(0xFF282828);         // #282828
                case "borderColor" -> new Color(0xFF4B4B4B);        // #4B4B4B
                case "surfaceColor" -> new Color(0xFF272727);       // #272727
                case "overlayColor" -> new Color(0x80000000);       // #000000
                case "cornerRadius" -> 4;
                case "defaultFont" -> new Font("/sys/fonts/default.font");
                default -> null;
            };
        }
        catch(IOException ex) {
            throw new UncheckedIOException(ex);
        }
    }
}
