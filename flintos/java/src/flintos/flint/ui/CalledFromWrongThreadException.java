package flint.ui;

public class CalledFromWrongThreadException extends RuntimeException {
    public CalledFromWrongThreadException() {
        super();
    }

    public CalledFromWrongThreadException(String msg) {
        super(msg);
    }
}
