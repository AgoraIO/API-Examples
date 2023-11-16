package io.agora.api.example.utils;

import android.content.Context;
import android.os.Process;

import java.io.IOException;
import java.io.InputStream;

/**
 * The type Audio file reader.
 */
public class AudioFileReader {
    private static final String AUDIO_FILE = "output.raw";
    /**
     * The constant SAMPLE_RATE.
     */
    public static final int SAMPLE_RATE = 44100;
    /**
     * The constant SAMPLE_NUM_OF_CHANNEL.
     */
    public static final int SAMPLE_NUM_OF_CHANNEL = 2;
    /**
     * The constant BITS_PER_SAMPLE.
     */
    public static final int BITS_PER_SAMPLE = 16;

    /**
     * The constant BYTE_PER_SAMPLE.
     */
    public static final float BYTE_PER_SAMPLE = 1.0f * BITS_PER_SAMPLE / 8 * SAMPLE_NUM_OF_CHANNEL;
    /**
     * The constant DURATION_PER_SAMPLE.
     */
    public static final float DURATION_PER_SAMPLE = 1000.0f / SAMPLE_RATE; // ms
    /**
     * The constant SAMPLE_COUNT_PER_MS.
     */
    public static final float SAMPLE_COUNT_PER_MS = SAMPLE_RATE * 1.0f / 1000; // ms

    private static final int BUFFER_SAMPLE_COUNT = (int) (SAMPLE_COUNT_PER_MS * 10); // 10ms sample count
    private static final int BUFFER_BYTE_SIZE = (int) (BUFFER_SAMPLE_COUNT * BYTE_PER_SAMPLE); // byte
    private static final long BUFFER_DURATION = (long) (BUFFER_SAMPLE_COUNT * DURATION_PER_SAMPLE); // ms

    private final Context context;
    private final OnAudioReadListener audioReadListener;
    private volatile boolean pushing = false;
    private InnerThread thread;
    private InputStream inputStream;

    /**
     * Instantiates a new Audio file reader.
     *
     * @param context  the context
     * @param listener the listener
     */
    public AudioFileReader(Context context, OnAudioReadListener listener) {
        this.context = context;
        this.audioReadListener = listener;
    }

    /**
     * Start.
     */
    public void start() {
        if (thread == null) {
            thread = new InnerThread();
            thread.start();
        }
    }

    /**
     * Stop.
     */
    public void stop() {
        pushing = false;
        if (thread != null) {
            try {
                thread.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            } finally {
                thread = null;
            }
        }
    }

    /**
     * The interface On audio read listener.
     */
    public interface OnAudioReadListener {
        /**
         * On audio read.
         *
         * @param buffer    the buffer
         * @param timestamp the timestamp
         */
        void onAudioRead(byte[] buffer, long timestamp);
    }

    private final class InnerThread extends Thread {

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

            long start_time = System.currentTimeMillis();
            int sent_audio_frames = 0;
            while (pushing) {
                if (audioReadListener != null) {
                    audioReadListener.onAudioRead(readBuffer(), System.currentTimeMillis());
                }
                ++sent_audio_frames;
                long next_frame_start_time = sent_audio_frames * BUFFER_DURATION + start_time;
                long now = System.currentTimeMillis();

                if (next_frame_start_time > now) {
                    long sleep_duration = next_frame_start_time - now;
                    try {
                        Thread.sleep(sleep_duration);
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
            int byteSize = BUFFER_BYTE_SIZE;
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
