package javax.microedition.lcdui;

public class Alert extends Screen {
    public static final int FOREVER = -2;
    private String text;
    private Image image;
    private AlertType type;
    private int timeout = FOREVER;

    public Alert(String title) {
        super(title);
    }

    public Alert(String title, String alertText, Image image, AlertType type) {
        super(title);
        this.text = alertText;
        this.image = image;
        this.type = type;
    }

    public String getString() {
        return text;
    }

    public void setString(String s) {
        text = s;
    }

    public Image getImage() {
        return image;
    }

    public void setImage(Image i) {
        image = i;
    }

    public AlertType getType() {
        return type;
    }

    public void setType(AlertType t) {
        type = t;
    }

    public int getTimeout() {
        return timeout;
    }

    public void setTimeout(int t) {
        timeout = t;
    }
}
