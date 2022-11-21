// Copyright (C) 2018 Beijing Bytedance Network Technology Co., Ltd.
package com.bytedance.labcv.core.util;

import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.util.Log;

import com.bytedance.labcv.effectsdk.BytedEffectConstants;

public class LogUtils {
    private static boolean isDebug = true;


    private static final String TAG = BytedEffectConstants.TAG;

    public static void v(String msg) {
        if (isDebug){
            Log.v(TAG, msg);
        }

    }

    public static void i(String msg)
    {
        if (isDebug){
            Log.i(TAG, msg);
        }
    }

    public static void d(String msg) {
        if (isDebug){
            Log.d(TAG, msg);

        }
    }

    public static void e(String msg) {
        Log.e(TAG, msg);
    }


    public static void syncIsDebug(Context context) {
            isDebug = context.getApplicationInfo() != null &&
                    (context.getApplicationInfo().flags & ApplicationInfo.FLAG_DEBUGGABLE) != 0;
            Log.d(TAG, "isDebug ="+isDebug);

    }
}

