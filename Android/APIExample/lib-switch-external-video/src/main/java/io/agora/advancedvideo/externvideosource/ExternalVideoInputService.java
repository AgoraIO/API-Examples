package io.agora.advancedvideo.externvideosource;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Intent;
import android.content.pm.ServiceInfo;
import android.os.Build;
import android.os.IBinder;
import android.os.RemoteException;

import androidx.annotation.Nullable;

public class ExternalVideoInputService extends Service
{
    private static final int NOTIFICATION_ID = 1;
    private static final String CHANNEL_ID = "ExternalVideo";

    private ExternalVideoInputManager mSourceManager;
    private IExternalVideoInputService mService;

    @Override
    public void onCreate()
    {
        super.onCreate();
        mSourceManager = new ExternalVideoInputManager(this.getApplicationContext());
        mService = new IExternalVideoInputService.Stub()
        {
            @Override
            public boolean setExternalVideoInput(int type, Intent intent) throws RemoteException
            {
                return mSourceManager.setExternalVideoInput(type, intent);
            }
        };
    }

    @Nullable
    @Override
    public IBinder onBind(Intent intent)
    {
        startForeground();
        startSourceManager();
        return mService.asBinder();
    }

    private void startForeground()
    {
        createNotificationChannel();

        Intent notificationIntent = new Intent(getApplicationContext(),
                getApplicationContext().getClass());
        PendingIntent pendingIntent = PendingIntent.getActivity(
                this, 0, notificationIntent, 0);

        Notification.Builder builder = new Notification.Builder(this)
                .setContentTitle(CHANNEL_ID)
                .setContentIntent(pendingIntent);

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O)
        {
            createNotificationChannel();
            builder.setChannelId(CHANNEL_ID);
        }

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q)
        {
            startForeground(NOTIFICATION_ID, builder.build(),
                    ServiceInfo.FOREGROUND_SERVICE_TYPE_MEDIA_PROJECTION);
        }
        else
        {
            startForeground(NOTIFICATION_ID, builder.build());
        }
    }

    private void createNotificationChannel()
    {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O)
        {
            int importance = NotificationManager.IMPORTANCE_DEFAULT;
            NotificationChannel channel = new NotificationChannel(
                    CHANNEL_ID, CHANNEL_ID, importance);
            channel.setDescription(CHANNEL_ID);
            NotificationManager notificationManager = getSystemService(NotificationManager.class);
            notificationManager.createNotificationChannel(channel);
        }
    }

    private void startSourceManager()
    {
        mSourceManager.start();
    }

    @Override
    public boolean onUnbind(Intent intent)
    {
        stopSourceManager();
        stopForeground(true);
        return true;
    }

    private void stopSourceManager()
    {
        if (mSourceManager != null)
        {
            mSourceManager.stop();
        }
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId)
    {
        return super.onStartCommand(intent, flags, startId);
    }

    @Override
    public void onDestroy()
    {
        super.onDestroy();
    }
}
