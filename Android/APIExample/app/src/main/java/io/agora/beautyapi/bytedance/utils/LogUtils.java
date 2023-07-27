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
package io.agora.beautyapi.bytedance.utils;

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

