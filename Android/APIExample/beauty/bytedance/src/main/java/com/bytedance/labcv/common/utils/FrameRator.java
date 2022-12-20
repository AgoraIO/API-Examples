// Copyright (C) 2018 Beijing Bytedance Network Technology Co., Ltd.
package com.bytedance.labcv.common.utils;

import android.os.Handler;
import android.os.Looper;
import android.os.Message;

import java.util.Deque;
import java.util.LinkedList;

public class FrameRator implements Handler.Callback {

    private static final int MSG_REFRESH = 0;

    private Deque<Long> mFrameStamps;

    private Handler mHandler;

    private int mFrameRate;

    public FrameRator() {
        mFrameStamps = new LinkedList<>();
        mHandler = new Handler(Looper.getMainLooper(), this);
    }

    public void start() {
        mHandler.removeCallbacksAndMessages(null);
        mHandler.sendEmptyMessage(MSG_REFRESH);
    }

    public void stop() {
        mHandler.removeCallbacksAndMessages(null);
        synchronized (mFrameStamps) {
            mFrameStamps.clear();
        }
    }

    public void addFrameStamp() {
        long now = System.currentTimeMillis();
        synchronized (mFrameStamps) {
            mFrameStamps.offerLast(now);
        }
    }

    private int calcFrameRate() {
        synchronized (mFrameStamps) {
            long now = System.currentTimeMillis();
            while (!mFrameStamps.isEmpty()) {
                long time = mFrameStamps.peekFirst();
                if (now - time > 1000 || mFrameStamps.size() > 30) {
                    mFrameStamps.pollFirst();
                } else {
                    break;
                }
            }
            return mFrameStamps.size();
        }
    }

    public int getFrameRate() {
        return mFrameRate;
    }

    @Override
    public boolean handleMessage(Message msg) {
        switch (msg.what) {
            case MSG_REFRESH:
                mFrameRate = calcFrameRate();
                mHandler.sendEmptyMessageDelayed(MSG_REFRESH, 1000);
                break;
        }

        return false;
    }
}
