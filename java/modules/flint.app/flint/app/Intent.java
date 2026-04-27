package flint.app;

import java.util.HashMap;
import java.util.Map;

public final class Intent {
    public static final String ACTION_MAIN = "flintos.intent.action.MAIN";
    public static final String ACTION_VIEW = "flintos.intent.action.VIEW";
    public static final String ACTION_SEND = "flintos.intent.action.SEND";
    public static final String ACTION_SETTINGS = "flintos.intent.action.SETTINGS";
    public static final String ACTION_APP_STORE = "flintos.intent.action.APP_STORE";
    public static final String ACTION_UPDATE = "flintos.intent.action.UPDATE";

    private String targetAppId;
    private String action;
    private String data;
    private final Map<String, String> extras = new HashMap<>();
    private final Map<String, Boolean> flags = new HashMap<>();

    public Intent() {}

    public static Intent explicit(String targetAppId) {
        return new Intent().setTargetAppId(targetAppId);
    }

    public static Intent action(String action) {
        return new Intent().setAction(action);
    }

    public String targetAppId() {
        return targetAppId;
    }

    public Intent setTargetAppId(String targetAppId) {
        this.targetAppId = targetAppId;
        return this;
    }

    public String action() {
        return action;
    }

    public Intent setAction(String action) {
        this.action = action;
        return this;
    }

    public String data() {
        return data;
    }

    public Intent setData(String data) {
        this.data = data;
        return this;
    }

    public Intent putExtra(String key, String value) {
        extras.put(key, value);
        return this;
    }

    public String extra(String key) {
        return extras.get(key);
    }

    public String extraOrDefault(String key, String defaultValue) {
        return extras.getOrDefault(key, defaultValue);
    }

    public Map<String, String> extras() {
        return new HashMap<>(extras);
    }

    public Intent addFlag(String flag) {
        return addFlag(flag, true);
    }

    public Intent addFlag(String flag, boolean value) {
        flags.put(flag, value);
        return this;
    }

    public boolean flag(String flag) {
        return flags.getOrDefault(flag, false);
    }

    public Map<String, Boolean> flags() {
        return new HashMap<>(flags);
    }
}
