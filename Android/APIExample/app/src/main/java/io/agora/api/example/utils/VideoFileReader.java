package io.agora.api.example.utils;

import android.content.Context;

import java.io.IOException;
import java.io.InputStream;

public class VideoFileReader {
    private final String RAW_VIDEO_PATH = "sample.yuv";
    private final int RAW_VIDEO_WIDTH = 320;
    private final int RAW_VIDEO_HEIGHT = 180;
    private final int RAW_VIDEO_FRAME_SIZE = RAW_VIDEO_WIDTH * RAW_VIDEO_HEIGHT / 2 * 3;
    private final int RAW_VIDEO_FRAME_RATE = 15;
    private final long RAW_VIDEO_FRAME_INTERVAL_NS = 1000 * 1000 * 1000 / RAW_VIDEO_FRAME_RATE;

    private volatile boolean pushing = false;
    private InputStream inputStream;

    private final Context context;
    private final OnVideoReadListener videoReadListener;
    private InnerThread thread;

    public VideoFileReader(Context context, OnVideoReadListener listener){
        this.context = context.getApplicationContext();
        this.videoReadListener = listener;
    }

    public final void start(){
        if(thread != null){
            return;
        }
        thread = new InnerThread();
        thread.start();
    }

    public final void stop(){
        if(thread != null){
            pushing = false;
            try {
                thread.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }finally {
                thread = null;
            }
        }
    }


    public interface OnVideoReadListener {
        void onVideoRead(byte[] buffer, int width, int height);
    }

    private class InnerThread extends Thread {
        @Override
        public void run() {
            super.run();
            try {
                inputStream = context.getAssets().open(RAW_VIDEO_PATH);
            } catch (IOException e) {
                e.printStackTrace();
            }

            pushing = true;
            byte[] buffer = new byte[RAW_VIDEO_FRAME_SIZE];
            while (pushing) {
                long start = System.nanoTime();
                try {
                    int read = inputStream.read(buffer);
                    while (read < 0) {
                        inputStream.reset();
                        read = inputStream.read(buffer);
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                }
                if(videoReadListener != null){
                    videoReadListener.onVideoRead(buffer, RAW_VIDEO_WIDTH, RAW_VIDEO_HEIGHT);
                }
                long consume = System.nanoTime() - start;

                try {
                    Thread.sleep(Math.max(0, (RAW_VIDEO_FRAME_INTERVAL_NS - consume) / 1000 / 1000));
                } catch (InterruptedException e) {
                    e.printStackTrace();
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
    }
}
