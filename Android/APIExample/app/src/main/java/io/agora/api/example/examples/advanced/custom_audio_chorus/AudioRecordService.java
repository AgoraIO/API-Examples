package io.agora.api.example.examples.advanced.customaudio;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Intent;
import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder;
import android.os.Build;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.IBinder;
import android.os.Looper;
import android.os.Message;
import android.util.Log;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.core.app.NotificationCompat;

import io.agora.api.example.MainActivity;

public class AudioRecordService extends Service
{
    private static final String TAG = AudioRecordService.class.getSimpleName();

    private RecordThread thread;
    private volatile boolean stopped;

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

    class ReverseHandler extends Handler {
        public ReverseHandler(Looper looper) {
            super(looper);
        }

        @Override
        public void handleMessage(@NonNull Message msg) {
            super.handleMessage(msg);
            byte[] bytes = (byte[]) msg.obj;
            Log.e(TAG, "tshi start pushCaptureAudioFrame at: " + System.nanoTime() / 1000);
            CustomAudioSource.engine .pushCaptureAudioFrame(bytes);
            Log.e(TAG, "tshi end pushCaptureAudioFrame at: " + System.nanoTime() / 1000);
        }
    }
    ReverseHandler mHandler;

    private void startRecording()
    {
        HandlerThread handlerThread = new HandlerThread("Reverse-Thread");
        handlerThread.start();
        mHandler = new ReverseHandler(handlerThread.getLooper());
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
        private AudioRecord audioRecord;
        public static final int DEFAULT_SAMPLE_RATE = 16000;
        /**1 corresponds to AudioFormat.CHANNEL_IN_MONO;
         * 2 corresponds to AudioFormat.CHANNEL_IN_STEREO*/
        public static final int DEFAULT_CHANNEL_COUNT = 1, DEFAULT_CHANNEL_CONFIG = AudioFormat.CHANNEL_IN_MONO;
        private byte[] buffer;

        RecordThread()
        {
            int bufferSize = AudioRecord.getMinBufferSize(DEFAULT_SAMPLE_RATE, DEFAULT_CHANNEL_CONFIG,
                    AudioFormat.ENCODING_PCM_16BIT);
            audioRecord = new AudioRecord(MediaRecorder.AudioSource.MIC, DEFAULT_SAMPLE_RATE, DEFAULT_CHANNEL_COUNT,
                    AudioFormat.ENCODING_PCM_16BIT, bufferSize);
            buffer = new byte[bufferSize];
        }

        @Override
        public void run()
        {
            try
            {
                audioRecord.startRecording();
                while (!stopped)
                {
                    int result = audioRecord.read(buffer, 0, buffer.length);
                    if (result >= 0) {
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
                        byte[] testReverse = new byte[buffer.length];
                        for (int i = 0; i < buffer.length - 1; ++i) {
                            testReverse[i] = (byte) (buffer[i] + 32);
                        }
                        Message message = Message.obtain();
                        message.obj = testReverse;
                        mHandler.sendMessageDelayed(message, 0);
                        Log.e(TAG, "tshi start pushReverseAudioFrame at: " + System.nanoTime() / 1000);
//                        CustomAudioSource.engine.pushReverseAudioFrame(buffer);
                        Log.e(TAG, "tshi stop pushReverseAudioFrame at: " + System.nanoTime() / 1000);
                    }
                    else
                    {
                        logRecordError(result);
                    }
                    Log.e(TAG, "数据大小:" + result);
                }
                release();
            }
            catch (Exception e)
            {e.printStackTrace();}
        }

        private void logRecordError(int error)
        {
            String message = "";
            switch (error)
            {
                case AudioRecord.ERROR:
                    message = "generic operation failure";
                    break;
                case AudioRecord.ERROR_BAD_VALUE:
                    message = "failure due to the use of an invalid value";
                    break;
                case AudioRecord.ERROR_DEAD_OBJECT:
                    message = "object is no longer valid and needs to be recreated";
                    break;
                case AudioRecord.ERROR_INVALID_OPERATION:
                    message = "failure due to the improper use of method";
                    break;
            }
            Log.e(TAG, message);
        }

        private void release()
        {
            if (audioRecord != null)
            {
                audioRecord.stop();
                buffer = null;
            }
        }
    }

    public native void initDemo();

    public native void startPulling();

    public native void startPushing();
}
