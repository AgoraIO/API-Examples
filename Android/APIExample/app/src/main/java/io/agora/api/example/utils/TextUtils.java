package io.agora.api.example.utils;

import android.content.Context;

import java.util.Locale;
import java.util.concurrent.TimeUnit;

public class TextUtils {
    /**
     * Print thread stack
     */
    public static String formatStackTrace(StackTraceElement[] stackTrace) {
        StringBuilder sb = new StringBuilder();
        for (StackTraceElement element : stackTrace) {
            sb.append("    at ").append(element.toString());
            sb.append("\n");
        }
        return sb.toString();
    }
    public static String durationFormat(Long duration) {
        long hours = TimeUnit.MILLISECONDS.toHours(duration)%60;
        long minutes =  TimeUnit.MILLISECONDS.toMinutes(duration)%60;
        long seconds = TimeUnit.MILLISECONDS.toSeconds(duration) % 60;
        if(hours>0)
            return String.format(Locale.getDefault(),"%02d:%02d:%02d",hours, minutes, seconds);
        else
            return String.format(Locale.getDefault(),"%02d:%02d", minutes, seconds);
    }
}
