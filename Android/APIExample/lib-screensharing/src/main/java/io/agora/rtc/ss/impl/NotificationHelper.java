package io.agora.rtc.ss.impl;

import android.annotation.TargetApi;
import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.content.Context;
import android.os.Build;

import androidx.annotation.RequiresApi;

public class NotificationHelper {

    public static String generateChannelId(Context ctx, int notification) {
        String channelId;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            channelId = NotificationHelper.createNotificationChannel(ctx, notification);
        } else {
            // If earlier version channel ID is not used
            // https://developer.android.com/reference/android/support/v4/app/NotificationCompat.Builder.html#NotificationCompat.Builder(android.content.Context)
            channelId = "";
        }
        return channelId;
    }

    @RequiresApi(Build.VERSION_CODES.O)
    @TargetApi(Build.VERSION_CODES.O)
    private static String createNotificationChannel(Context ctx, int notification) {


        String channelId;
        String channelName;

        NotificationChannel chan;

        switch (notification) {
            default:
                channelId = "generic_noti";
                channelName = "Generic";

                chan = new NotificationChannel(channelId,
                        channelName, NotificationManager.IMPORTANCE_NONE);
                break;

        }

        chan.setLockscreenVisibility(Notification.VISIBILITY_PRIVATE);
        NotificationManager service = (NotificationManager) ctx.getSystemService(Context.NOTIFICATION_SERVICE);
        service.createNotificationChannel(chan);
        return channelId;
    }
}
