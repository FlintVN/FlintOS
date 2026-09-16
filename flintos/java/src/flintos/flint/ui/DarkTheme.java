package flint.ui;

import flint.drawing.Color;

public class DarkTheme extends Theme {
    private final static Color backgroundColor = new Color(0xFF121212);     // #121212
    private final static Color primaryColor = new Color(0xFF2196F3);        // #2196F3
    private final static Color secondaryColor = new Color(0xFF9C27B0);      // #9C27B0
    private final static Color accentColor = new Color(0xFF03A9F4);         // #03A9F4
    private final static Color textColor = new Color(0xFFE0E0E0);           // #E0E0E0
    private final static Color secondaryTextColor = new Color(0xFFAAAAAA);  // #AAAAAA
    private final static Color disabledTextColor = new Color(0xFF5A5A5A);   // #5A5A5A
    private final static Color hintTextColor = new Color(0xFF787878);       // #787878
    private final static Color disabledColor = new Color(0xFF373737);       // #373737
    private final static Color pressedColor = new Color(0xFF1976BE);        // #1976BE
    private final static Color focusedColor = new Color(0xFF42A5F5);        // #42A5F5
    private final static Color selectedColor = new Color(0xFF2D2D2D);       // #2D2D2D
    private final static Color hoverColor = new Color(0xFF282828);          // #282828
    private final static Color borderColor = new Color(0xFF4B4B4B);         // #4B4B4B
    private final static Color surfaceColor = new Color(0xFF1E1E1E);        // #1E1E1E
    private final static Color overlayColor = new Color(0x80000000);        // #000000

    public DarkTheme() {

    }

    public Color backgroundColor() {
        return backgroundColor;
    }

    public Color primaryColor() {
        return primaryColor;
    }

    public Color secondaryColor() {
        return secondaryColor;
    }

    public Color accentColor() {
        return accentColor;
    }

    public Color textColor() {
        return textColor;
    }
    
    public Color secondaryTextColor() {
        return secondaryTextColor;
    }

    public Color disabledTextColor() {
        return disabledTextColor;
    }

    public Color hintTextColor() {
        return hintTextColor;
    }
    
    public Color disabledColor() {
        return disabledColor;
    }

    public Color pressedColor() {
        return pressedColor;
    }

    public Color focusedColor() {
        return focusedColor;
    }

    public Color selectedColor() {
        return selectedColor;
    }

    public Color hoverColor() {
        return hoverColor;
    }

    public Color borderColor() {
        return borderColor;
    }

    public Color surfaceColor() {
        return surfaceColor;
    }

    public Color overlayColor() {
        return overlayColor;
    }
}
