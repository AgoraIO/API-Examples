package io.agora.api.example.utils;

/**
 * The type Text utils.
 */
public final class TextUtils {

    private TextUtils() {

    }
    /**
     * Format stack trace string.
     *
     * @param stackTrace the stack trace
     * @return the string
     */
    public static String formatStackTrace(StackTraceElement[] stackTrace) {
        StringBuilder sb = new StringBuilder();
        for (StackTraceElement element : stackTrace) {
            sb.append("    at ").append(element.toString());
            sb.append("\n");
        }
        return sb.toString();
    }
}
