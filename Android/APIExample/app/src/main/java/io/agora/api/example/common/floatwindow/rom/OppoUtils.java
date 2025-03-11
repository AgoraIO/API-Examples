package io.agora.api.example.common.floatwindow.rom;

import android.app.AppOpsManager;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.os.Binder;
import android.util.Log;

import java.lang.reflect.Method;

/**
 * The type Oppo utils.
 */
public final class OppoUtils {

    private static final String TAG = "OppoUtils";

    private OppoUtils() {

    }

    /**
     * 检测 360 悬浮窗权限
     *
     * @param context the context
     * @return the boolean
     */
    public static boolean checkFloatWindowPermission(Context context) {
        return checkOp(context, 24); //OP_SYSTEM_ALERT_WINDOW = 24;
    }

    private static boolean checkOp(Context context, int op) {
        AppOpsManager manager = (AppOpsManager) context.getSystemService(Context.APP_OPS_SERVICE);
        try {
            Class clazz = AppOpsManager.class;
            Method method = clazz.getDeclaredMethod("checkOp", int.class, int.class, String.class);
            return AppOpsManager.MODE_ALLOWED == (int) method.invoke(manager, op, Binder.getCallingUid(), context.getPackageName());
        } catch (Exception e) {
            Log.e(TAG, Log.getStackTraceString(e));
        }
        return false;
    }

    /**
     * oppo ROM 权限申请
     *
     * @param context the context
     */
    public static void applyOppoPermission(Context context) {
        //merge request from https://github.com/zhaozepeng/FloatWindowPermission/pull/26
        try {
            Intent intent = new Intent();
            intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            //com.coloros.safecenter/.sysfloatwindow.FloatWindowListActivity
            ComponentName comp = new ComponentName("com.coloros.safecenter", "com.coloros.safecenter.sysfloatwindow.FloatWindowListActivity"); //悬浮窗管理页面
            intent.setComponent(comp);
            context.startActivity(intent);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
}
