package io.agora.api.example.examples.advanced.customaudio;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.media.AudioFormat;
import android.os.Build;
import android.os.Environment;
import android.os.IBinder;
import android.util.Log;

import androidx.annotation.Nullable;
import androidx.core.app.NotificationCompat;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.nio.ByteBuffer;

import io.agora.api.example.MainActivity;

public class PCMRecordService extends Service
{
    private static final String TAG = PCMRecordService.class.getSimpleName();
    public static final String FILE_NAME = "bgm.pcm";
    private Context context;

    private RecordThread thread;
    private volatile boolean stopped;

    @Override
    public void onCreate() {
        super.onCreate();
        context = this; //Returns null when service is running on background
    }

    @Nullable
    @Override
    public IBinder onBind(Intent intent)
    {
        return null;
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId)
    {
        startForeground();
        startRecording();
        return Service.START_STICKY;
    }

    private void startForeground()
    {
        createNotificationChannel();
        Intent notificationIntent = new Intent(this, MainActivity.class);
        PendingIntent pendingIntent = PendingIntent.getActivity(this,
                0, notificationIntent, 0);

        Notification notification = new NotificationCompat.Builder(this, TAG)
                .setContentTitle(TAG)
                .setContentIntent(pendingIntent)
                .build();

        startForeground(1, notification);
    }

    private void createNotificationChannel()
    {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O)
        {
            NotificationChannel serviceChannel = new NotificationChannel(
                    TAG, TAG, NotificationManager.IMPORTANCE_DEFAULT
            );

            NotificationManager manager = getSystemService(NotificationManager.class);
            manager.createNotificationChannel(serviceChannel);
        }
    }

    private void startRecording()
    {
        thread = new RecordThread();
        thread.start();
    }

    private void stopRecording()
    {
        stopped = true;
    }

    @Override
    public void onDestroy()
    {
        stopRecording();
        super.onDestroy();
    }

    public class RecordThread extends Thread
    {
        public static final int DEFAULT_SAMPLE_RATE = 44100;
        /**1 corresponds to AudioFormat.CHANNEL_IN_MONO;
         * 2 corresponds to AudioFormat.CHANNEL_IN_STEREO*/
        public static final int DEFAULT_CHANNEL_COUNT = 2, DEFAULT_CHANNEL_CONFIG = AudioFormat.CHANNEL_IN_MONO;
        private int channelsPerFrame = DEFAULT_CHANNEL_COUNT;
        private int bitPerSample = 16;
        private int samples = 441 * 20;
        private File audioFile;
        private int frameSize;

        private void initSource() {
            File dir = context.getExternalFilesDir(Environment.DIRECTORY_MUSIC);
            audioFile = new File(dir, PCMRecordService.FILE_NAME);
            frameSize = samples * bitPerSample / 8 * channelsPerFrame;
            stopped = false;
        }

        private void readFile(File file) {
            try {
                if (file.isFile()) {
                    FileInputStream fis = null;
                    try {
                        fis = new FileInputStream(file);

                        int len = 0;
                        while (!stopped) {
                            int randomSamples = samples;
                            frameSize = randomSamples * bitPerSample / 8 * channelsPerFrame;
                            stopped = false;
                            byte[] buffer = new byte[frameSize];
                            if ((len = fis.read(buffer)) == -1) continue;
                            Log.d("dqm", "frameSize: " + frameSize + "  len: " + len);
                            try {
                                /**Pushes the external audio frame to the Agora SDK for encoding.
                                 * @param data External audio data to be pushed.
                                 * @param timeStamp Timestamp of the external audio frame. It is mandatory.
                                 *                  You can use this parameter for the following purposes:
                                 *                    1:Restore the order of the captured audio frame.
                                 *                    2:Synchronize audio and video frames in video-related
                                 *                      scenarios, including scenarios where external video sources are used.
                                 * @return
                                 *   0: Success.
                                 *   < 0: Failure.*/
                                CustomAudioSource.engine.pushExternalAudioFrame(
                                        ByteBuffer.wrap(buffer), 0, CustomAudioSource.rtcConnection2.localUid);
                            } catch (Exception e) {
                                e.printStackTrace();
                            }
                        }
                    } catch (IOException e) {
                        e.printStackTrace();
                    } finally {
                        if (fis != null) {
                            try {
                                fis.close();
                            } catch (IOException e) {
                                e.printStackTrace();
                            }
                        }
                    }
                } else {
                    throw new Exception("Audio File not found : " + FILE_NAME);
                }
            } catch (Exception e) {
            }

        }

        @Override
        public void run()
        {
            try
            {
                initSource();
                readFile(this.audioFile);
            }
            catch (Exception e)
            {e.printStackTrace();}
        }

    }
}
