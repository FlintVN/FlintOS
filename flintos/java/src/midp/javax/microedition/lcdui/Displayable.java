package javax.microedition.lcdui;

public abstract class Displayable {
    Command commands[];
    int numCommands;
    CommandListener listener;
    boolean isInFullScreenMode;
    boolean isRotated;
    String title;

    Displayable() {

    }

    Displayable(String title) {
        this.title = title;
    }

    public String getTitle() {
        return title;
    }

    public void setTitle(String s) {
        this.title = s;
    }

    public boolean isShown() {
        return Display.currentShown() == this;
    }

    public void addCommand(Command cmd) {
        if(cmd == null)
            throw new NullPointerException();
        synchronized(Display.LCDUILock) {
            addCommandImpl(cmd);
        }
    }

    public void removeCommand(Command cmd) {
        synchronized(Display.LCDUILock) {
            removeCommandImpl(cmd);
        }
    }

    public void setCommandListener(CommandListener l) {
        listener = l;
    }

    public int getWidth() {
        return Display.getPrimaryDisplayWidth();
    }

    public int getHeight() {
        return Display.getPrimaryDisplayHeight();
    }

    protected void sizeChanged(int w, int h) {

    }

    int addCommandImpl(Command cmd) {
        for(int i = 0; i < numCommands; ++i) {
            if(commands[i] == cmd)
                return -1;
        }

        if((commands == null) || (numCommands == commands.length)) {
            Command[] newCommands = new Command[numCommands + 4];
            if(commands != null)
                System.arraycopy(commands, 0, newCommands, 0, numCommands);
            commands = newCommands;
        }

        commands[numCommands] = cmd;
        ++numCommands;

        return numCommands - 1;
    }

    int removeCommandImpl(Command cmd) {
        for(int i = 0; i < numCommands; ++i) {
            if(commands[i] == cmd) {
                commands[i] = commands[--numCommands];
                commands[numCommands] = null;
                return i;
            }
        }
        return -1;
    }
}
