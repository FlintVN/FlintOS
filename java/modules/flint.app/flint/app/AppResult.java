package flint.app;

public final class AppResult {
    public static final String OK = "OK";
    public static final String CANCEL = "CANCEL";

    private final String resultCode;
    private final String data;

    public AppResult(String resultCode, String data) {
        this.resultCode = resultCode;
        this.data = data;
    }

    public String resultCode() {
        return resultCode;
    }

    public String data() {
        return data;
    }
}
