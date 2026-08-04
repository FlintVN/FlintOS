package javax.microedition.lcdui;

public class Command {
    public static final int SCREEN = 1;
    public static final int BACK = 2;
    public static final int CANCEL = 3;
    public static final int OK = 4;
    public static final int HELP = 5;
    public static final int STOP = 6;
    public static final int EXIT = 7;
    public static final int ITEM = 8;
    static final int VIRTUAL = 9;

    String shortLabel;
    String longLabel;
    int commandType;
    int priority;

    private int id;

    public Command(String label, int commandType, int inp_priority) {
        this(label, null, commandType, inp_priority);
    }

    public Command(String shortLabel, String longLabel, int commandType, int inp_priority) {
        initialize(commandType, inp_priority);
        setLabel(shortLabel, longLabel);
    }

    public String getLabel() {
        return shortLabel;
    }

    public String getLongLabel() {
        return longLabel;
    }

    public int getCommandType() {
        return commandType;
    }

    public int getPriority() {
        return priority;
    }

    int getID() {
        return id;
    }

    void setInternalID(int num) {
        this.id = num;
    }

    private void setLabel(String shortLabel, String longLabel) {
        if(shortLabel == null)
            throw new NullPointerException();
        this.shortLabel = shortLabel;
        this.longLabel = longLabel;
    }

    private final void initialize(int commandType, int inp_priority) {
        if((commandType < SCREEN) || (commandType > ITEM))
            throw new IllegalArgumentException();

        this.commandType = commandType;
        priority = inp_priority;
    }
}
