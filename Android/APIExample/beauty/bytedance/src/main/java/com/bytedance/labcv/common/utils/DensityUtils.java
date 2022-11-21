package com.bytedance.labcv.common.utils;

import android.app.Activity;
import android.content.Context;
import android.graphics.Rect;
import android.view.Window;
import android.view.WindowManager;

public class DensityUtils {
    public static float dp2px(Context context, float dp) {
        float scale = context.getResources().getDisplayMetrics().density;
        return dp * scale + 0.5F;
    }

    public static float px2dp(Context context, float px) {
        float scale = context.getResources().getDisplayMetrics().density;
        return px / scale + 0.5F;
    }

    public static int sp2px(Context context, float sp) {
        float scale = context.getResources().getDisplayMetrics().scaledDensity;
        return (int) (sp * scale + 0.5f);
    }

    public static int px2sp(Context context, float px) {
        float scale = context.getResources().getDisplayMetrics().scaledDensity;
        return (int) (px / scale + 0.5f);
    }

//    public static int getStatusHeight(Context context) {
//        int resourceId = context.getResources().getIdentifier("status_bar_height",
//                "dimen", "android");
//        return context.getResources().getDimensionPixelSize(resourceId);
//    }

    public static int getStatusBarHeight(Activity activity){
        Rect rectangle = new Rect();
        Window window = activity.getWindow();
        window.getDecorView().getWindowVisibleDisplayFrame(rectangle);
        int statusBarHeight = rectangle.top;
        int contentViewTop =
                window.findViewById(Window.ID_ANDROID_CONTENT).getTop();
        int titleBarHeight= contentViewTop - statusBarHeight;
        return Math.abs(titleBarHeight);
    }

    public static int getScreenWidth(Activity context){
        WindowManager wm = context.getWindowManager();
        return wm.getDefaultDisplay().getWidth();
    }

    public static int getScreenHeight(Activity context){
        WindowManager wm = context.getWindowManager();
        return wm.getDefaultDisplay().getHeight();
    }
}
