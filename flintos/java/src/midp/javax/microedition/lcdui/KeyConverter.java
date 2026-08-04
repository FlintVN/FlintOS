package javax.microedition.lcdui;

class KeyConverter {
    public static native int getKeyCode(int gameAction);

    public static native int getGameAction(int keyCode);

    public static native int getSystemKey(int keyCode);

    public static native String getKeyName(int keyCode);
}
