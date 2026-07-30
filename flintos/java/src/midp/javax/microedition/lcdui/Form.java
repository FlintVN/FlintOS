package javax.microedition.lcdui;

public class Form extends Screen {
    public Form(String title) {
        super(title);
    }

    public Form(String title, Item[] items) {
        super(title);
    }

    public int append(Item item) {
        return 0;
    }

    public int append(String str) {
        return 0;
    }

    public int append(Image img) {
        return 0;
    }

    public void delete(int i) {

    }

    public void deleteAll() {

    }

    public int size() {
        return 0;
    }
}
