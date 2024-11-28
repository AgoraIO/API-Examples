package io.agora.api.example.service;// MediaProjectionService.java

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.content.pm.ApplicationInfo;
import android.content.pm.ServiceInfo;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Build;
import android.os.IBinder;
import android.util.Log;

import androidx.annotation.Nullable;

import io.agora.api.example.R;

public class MediaProjectionService extends Service {
    private static final int NOTIFICATION_ID = 98779;
    public static final String CHANNEL_ID = "MediaProjectionServiceChannel";
    private static final String TAG = MediaProjectionService.class.getSimpleName();

    @Override
    public void onCreate() {
        super.onCreate();

    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Notification notification = getDefaultNotification();

        try {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
                this.startForeground(NOTIFICATION_ID, notification, ServiceInfo.FOREGROUND_SERVICE_TYPE_MEDIA_PROJECTION);
            } else {
                this.startForeground(NOTIFICATION_ID, notification);
            }
        } catch (Exception ex) {
            Log.e(TAG, "", ex);
        }
        return START_STICKY;
    }

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    private Notification getDefaultNotification() {
        ApplicationInfo appInfo = this.getApplicationContext().getApplicationInfo();
        String name = this.getApplicationContext().getPackageManager().getApplicationLabel(appInfo).toString();
        int icon = appInfo.icon;

        try {
            Bitmap iconBitMap = BitmapFactory.decodeResource(this.getApplicationContext().getResources(), icon);
            if (iconBitMap == null || iconBitMap.getByteCount() == 0) {
                Log.w(TAG, "Couldn't load icon from icon of applicationInfo, use android default");
                icon = R.mipmap.ic_launcher;
            }
        } catch (Exception ex) {
            Log.w(TAG, "Couldn't load icon from icon of applicationInfo, use android default");
            icon = R.mipmap.ic_launcher;
        }

        if (Build.VERSION.SDK_INT >= 26) {
            NotificationChannel mChannel = new NotificationChannel(CHANNEL_ID, name, NotificationManager.IMPORTANCE_DEFAULT);
            NotificationManager mNotificationManager = (NotificationManager) this.getSystemService(Context.NOTIFICATION_SERVICE);
            mNotificationManager.createNotificationChannel(mChannel);
        }


        Notification.Builder builder = new Notification.Builder(this)
                .setContentText("Screen Sharing ...")
                .setOngoing(true)
                .setPriority(Notification.PRIORITY_HIGH)
                .setSmallIcon(icon)
                .setTicker(name)
                .setWhen(System.currentTimeMillis());
        if (Build.VERSION.SDK_INT >= 26) {
            builder.setChannelId(CHANNEL_ID);
        }

        return builder.build();
    }
}