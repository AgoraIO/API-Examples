package io.agora.api.example.utils;

import android.content.Context;
import android.os.Process;

import java.io.IOException;
import java.io.InputStream;

public class AudioFileReader {
    private static final String AUDIO_FILE = "output.raw";
    public static final Integer SAMPLE_RATE = 44100;
    public static final Integer SAMPLE_NUM_OF_CHANNEL = 2;
    public static final Integer BITS_PER_SAMPLE = 16;
    private static final Integer SAMPLES = 441;
    private static final Integer BUFFER_SIZE = SAMPLES * BITS_PER_SAMPLE / 8 * SAMPLE_NUM_OF_CHANNEL;
    private static final Integer PUSH_INTERVAL = SAMPLES * 1000 / SAMPLE_RATE;

    private final Context context;
    private final OnAudioReadListener audioReadListener;
    private volatile boolean pushing = false;
    private InnerThread thread;
    private InputStream inputStream;

    public AudioFileReader(Context context, OnAudioReadListener listener){
        this.context = context;
        this.audioReadListener = listener;
    }

    public void start() {
        if(thread == null){
            thread = new InnerThread();
            thread.start();
        }
    }

    public void stop(){
        pushing = false;
        if(thread != null){
            try {
                thread.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            } finally {
                thread = null;
            }
        }
    }

    public interface OnAudioReadListener {
        void onAudioRead(byte[] buffer, long timestamp);
    }

    private class InnerThread extends Thread{

        @Override
        public void run() {
            super.run();
            try {
                inputStream = context.getAssets().open(AUDIO_FILE);
            } catch (IOException e) {
                e.printStackTrace();
            }
            Process.setThreadPriority(Process.THREAD_PRIORITY_URGENT_AUDIO);
            pushing = true;
            while (pushing) {
                long before = System.currentTimeMillis();
                if(audioReadListener != null){
                    audioReadListener.onAudioRead(readBuffer(), System.currentTimeMillis());
                }
                long now = System.currentTimeMillis();
                long consuming = now - before;
                if(consuming < PUSH_INTERVAL){
                    try {
                        Thread.sleep(PUSH_INTERVAL - consuming);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
            }

            if (inputStream != null) {
                try {
                    inputStream.close();
                } catch (IOException e) {
                    e.printStackTrace();
                } finally {
                    inputStream = null;
                }
            }
        }

        private byte[] readBuffer() {
            int byteSize = BUFFER_SIZE;
            byte[] buffer = new byte[byteSize];
            try {
                if (inputStream.read(buffer) < 0) {
                    inputStream.reset();
                    return readBuffer();
                }
            } catch (IOException e) {
                e.printStackTrace();
            }
            return buffer;
        }
    }
}
