package javax.microedition.lcdui;

public class AlertType {
    public static final AlertType INFO = new AlertType(0);
    public static final AlertType WARNING = new AlertType(1);
    public static final AlertType ERROR = new AlertType(2);
    public static final AlertType ALARM = new AlertType(3);
    public static final AlertType CONFIRMATION = new AlertType(4);

    private int type;

    protected AlertType() {

    }

    public boolean playSound(Display display) {
        // TODO
        return false;
    }

    AlertType(int type) {
        this.type = type;
    }

    int getType() {
        return this.type;
    }
}
