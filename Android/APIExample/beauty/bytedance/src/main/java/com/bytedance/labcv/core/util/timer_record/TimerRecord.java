package com.bytedance.labcv.core.util.timer_record;

import android.util.Log;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

/**
 * Created on 2020-02-06 19:24
 */
public abstract class TimerRecord {
    protected static final String TAG = "TimerRecord";

    private HashMap<String, List<Long>> mRecord;
    private HashMap<String, Long> mStartTime;

    private boolean mEnable;
    private int mMaxRound;
    private boolean mShowTime;
    private boolean mShowAverage;

    public TimerRecord() {
        this(2000, false, true);
    }

    public TimerRecord(int maxRound, boolean showTime, boolean showAverage) {
        mRecord = new HashMap<>();
        mStartTime = new HashMap<>();
        mEnable = true;
        mMaxRound = maxRound;
        mShowTime = showTime;
        mShowAverage = showAverage;
    }

    public void setEnable(boolean enable) {
        this.mEnable = enable;
    }

    public void setShowTime(boolean showTime) {
        this.mShowTime = showTime;
    }

    public void setShowAverage(boolean showAverage) {
        this.mShowAverage = showAverage;
    }

    public void setMaxRound(int maxRound) {
        this.mMaxRound = maxRound;
    }

    public void record(String tag) {
        if (!mEnable) return;
        long cur = System.nanoTime();
        mStartTime.put(tag, cur);
    }

    public void stop(String tag) {
        if (!mEnable) return;
        Long last = mStartTime.get(tag);
        if (last == null) {
            Log.i(TAG, String.format("call record() with tag %s first", tag));
            return;
        }
        long time = System.nanoTime() - last;
        if (mRecord.get(tag) == null) {
            mRecord.put(tag, new ArrayList<Long>(mMaxRound));
        }
        ArrayList<Long> list = (ArrayList<Long>) mRecord.get(tag);
        list.add(time);
        if (mShowTime) {
            recordOnce(tag, time);
        }
        if (list.size() == mMaxRound) {
            if (mShowAverage) {
                double average = 0;
                for (long l : list) {
                    average += l;
                }
                average /= mMaxRound;
                recordAverage(tag, average, mMaxRound);
            }
            list.clear();
        }
    }

    public void clear(String tag) {
        List list = mRecord.get(tag);
        if (list != null) {
            list.clear();
        }
        mStartTime.remove(tag);
    }

    public void clear() {
        mStartTime.clear();
        mRecord.clear();
    }

    protected abstract void recordOnce(String tag, long time);
    protected abstract void recordAverage(String tag, double time, int round);
}
