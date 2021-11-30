package io.agora.api.example.examples.advanced.customaudio;

import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.Service;
import android.content.Intent;
import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder;
import android.os.Binder;
import android.os.Build;
import android.os.IBinder;
import android.util.Log;

import androidx.annotation.Nullable;
import androidx.core.app.NotificationCompat;

import io.agora.rtc.Constants.AudioExternalSourcePos;
import io.agora.rtc.RtcEngine;

public class AudioRecordService extends Service {
    private static final String TAG = AudioRecordService.class.getSimpleName();

    private Thread recordThread;
    private RecordBinder recordBinder;

    /**
     * Since v3.5.1
     * https://docs.agora.io/en/Interactive%20Broadcast/API%20Reference/java/classio_1_1agora_1_1rtc_1_1_rtc_engine.html?platform=Android#acc77cfbe4ddfcf36d722a89af6729ce5
     *
     * {@link io.agora.rtc.Constants.AudioExternalSourcePos}
     * AUDIO_EXTERNAL_PLAYOUT_SOURCE(0)
     * AUDIO_EXTERNAL_RECORD_SOURCE_PRE_PROCESS(1),
     * AUDIO_EXTERNAL_RECORD_SOURCE_POST_PROCESS(2);
     *
     */
    public volatile int currentSourcePos = AudioExternalSourcePos.getValue(AudioExternalSourcePos.AUDIO_EXTERNAL_PLAYOUT_SOURCE);

    public RtcEngine engine = null;
    @Override
    public void onCreate() {
        super.onCreate();
        recordThread = new RecordThread();
        recordBinder = new RecordBinder();
        startForeground();
    }

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return recordBinder;
    }


    private void startForeground() {
        createNotificationChannel();
        Notification notification = new NotificationCompat.Builder(this, TAG)
                .setContentTitle(TAG)
                .build();

        startForeground(1, notification);
    }

    private void createNotificationChannel() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            NotificationChannel serviceChannel = new NotificationChannel(
                    TAG, TAG, NotificationManager.IMPORTANCE_HIGH
            );

            NotificationManager manager = getSystemService(NotificationManager.class);
            manager.createNotificationChannel(serviceChannel);
        }
    }

    public void startRecording() {
        recordThread.start();
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        recordThread.interrupt();
    }

    public class RecordThread extends Thread {
        private final AudioRecord audioRecord;
        public static final int DEFAULT_SAMPLE_RATE = 44100;
        /**
         * 1 corresponds to AudioFormat.CHANNEL_IN_MONO;
         * 2 corresponds to AudioFormat.CHANNEL_IN_STEREO
         */
        public static final int DEFAULT_CHANNEL_COUNT = 1, DEFAULT_CHANNEL_CONFIG = AudioFormat.CHANNEL_IN_MONO;
        private byte[] buffer;

        RecordThread() {
            int bufferSize = AudioRecord.getMinBufferSize(DEFAULT_SAMPLE_RATE, DEFAULT_CHANNEL_CONFIG,
                    AudioFormat.ENCODING_PCM_16BIT);
            audioRecord = new AudioRecord(MediaRecorder.AudioSource.MIC, DEFAULT_SAMPLE_RATE, DEFAULT_CHANNEL_COUNT,
                    AudioFormat.ENCODING_PCM_16BIT, bufferSize);
            buffer = new byte[bufferSize];
        }

        @Override
        public void run() {
            audioRecord.startRecording();
            while (!isInterrupted()) {
                int result = audioRecord.read(buffer, 0, buffer.length);
                if (result > 0 && engine != null) {
                    engine.pushExternalAudioFrame(buffer, System.currentTimeMillis(), DEFAULT_SAMPLE_RATE, DEFAULT_CHANNEL_COUNT
                            , AudioFormat.ENCODING_PCM_16BIT, AudioRecordService.this.currentSourcePos);
                } else {
                    logRecordError(result);
                }
                Log.d(TAG, "byte size is :" + result);
            }
            release();
        }

        private void logRecordError(int error) {
            String message = "";
            switch (error) {
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

        private void release() {
            if (audioRecord != null) {
                audioRecord.stop();
                audioRecord.release();
            }
            buffer = null;
        }
    }

    public class RecordBinder extends Binder {

        public AudioRecordService getService(){
            return AudioRecordService.this;
        }
    }
}
