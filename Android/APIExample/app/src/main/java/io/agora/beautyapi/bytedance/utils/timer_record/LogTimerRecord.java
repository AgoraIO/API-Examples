/*
 * MIT License
 *
 * Copyright (c) 2023 Agora Community
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

package io.agora.beautyapi.bytedance.utils.timer_record;

import android.util.Log;

/**
 * Created on 2020-02-07 10:37
 */
public class LogTimerRecord extends TimerRecord {
    private volatile static LogTimerRecord sInstance;

    private static LogTimerRecord getInstance() {
        if (sInstance == null) {
            synchronized (LogTimerRecord.class) {
                if (sInstance == null) {
                    sInstance = new LogTimerRecord(1000, true, false);
                    sInstance.setEnable(false); // 关闭日志打印
                }
            }
        }
        return sInstance;
    }

    public static void RECORD(String tag) {
        getInstance().record(tag);
    }

    public static void STOP(String tag) {
        getInstance().stop(tag);
    }

    public LogTimerRecord() {
        super();
    }

    public LogTimerRecord(int maxRound, boolean showTime, boolean showAverage) {
        super(maxRound, showTime, showAverage);
    }

    @Override
    protected void recordOnce(String tag, long time) {
        Log.i(TAG, String.format("%s %f", tag, time * 0.000001));
    }

    @Override
    protected void recordAverage(String tag, double time, int round) {
        Log.i(TAG, String.format("%s average time is %f ms in %d round", tag, time * 0.000001, round));
    }
}
