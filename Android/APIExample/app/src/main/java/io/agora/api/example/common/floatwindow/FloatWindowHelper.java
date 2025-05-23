package io.agora.api.example.common.floatwindow;

import android.app.AlertDialog;
import android.content.Context;
import android.content.Intent;
import android.graphics.PixelFormat;
import android.graphics.Point;
import android.net.Uri;
import android.os.Build;
import android.provider.Settings;
import android.util.Log;
import android.view.Gravity;
import android.view.WindowManager;

import androidx.annotation.NonNull;

import java.lang.reflect.Field;
import java.lang.reflect.Method;

import io.agora.api.example.R;
import io.agora.api.example.common.floatwindow.rom.HuaweiUtils;
import io.agora.api.example.common.floatwindow.rom.MeizuUtils;
import io.agora.api.example.common.floatwindow.rom.MiuiUtils;
import io.agora.api.example.common.floatwindow.rom.OppoUtils;
import io.agora.api.example.common.floatwindow.rom.QikuUtils;
import io.agora.api.example.common.floatwindow.rom.RomUtils;

/**
 * The type Float window helper.
 */
public final class FloatWindowHelper {
    private static final String TAG = "FloatWindowHelper";

    private FloatWindowHelper() {

    }

    /**
     * Create float view av call float view.
     *
     * @param context the context
     * @param xDp     the x dp
     * @param yDp     the y dp
     * @return the av call float view
     */
    public static AVCallFloatView createFloatView(@NonNull Context context, int xDp, int yDp) {
        WindowManager windowManager = (WindowManager) context.getApplicationContext().getSystemService(Context.WINDOW_SERVICE);
        Point size = new Point();
        windowManager.getDefaultDisplay().getSize(size);
        int screenWidth = size.x;
        int screenHeight = size.y;

        WindowManager.LayoutParams params = new WindowManager.LayoutParams();
        params.packageName = context.getPackageName();
        params.width = WindowManager.LayoutParams.WRAP_CONTENT;
        params.height = WindowManager.LayoutParams.WRAP_CONTENT;
        params.flags = WindowManager.LayoutParams.FLAG_NOT_TOUCH_MODAL
                | WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE | WindowManager.LayoutParams.FLAG_LAYOUT_INSET_DECOR
                | WindowManager.LayoutParams.FLAG_LAYOUT_IN_SCREEN;
        int mType;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            mType = WindowManager.LayoutParams.TYPE_APPLICATION_OVERLAY;
        } else {
            mType = WindowManager.LayoutParams.TYPE_SYSTEM_ERROR;
        }
        params.type = mType;
        params.format = PixelFormat.RGBA_8888;
        params.gravity = Gravity.START | Gravity.TOP;
        params.x = screenWidth - dp2px(context, xDp);
        params.y = screenHeight - dp2px(context, yDp);

        AVCallFloatView floatView = new AVCallFloatView(context);
        floatView.setParams(params);
        floatView.setIsShowing(true);
        windowManager.addView(floatView, params);
        return floatView;
    }

    /**
     * Destroy float view.
     *
     * @param floatView the float view
     */
    public static void destroyFloatView(@NonNull AVCallFloatView floatView) {
        WindowManager windowManager = (WindowManager) floatView.getContext().getApplicationContext().getSystemService(Context.WINDOW_SERVICE);
        floatView.setIsShowing(false);
        windowManager.removeViewImmediate(floatView);
    }

    /**
     * Check permission boolean.
     *
     * @param context the context
     * @return the boolean
     */
    public static boolean checkPermission(Context context) {
        // After Android 6.0, Google added management for floating window permissions, so the method is unified
        return commonROMPermissionCheck(context);
    }

    /**
     * Apply permission.
     *
     * @param context the context
     */
    public static void applyPermission(Context context) {
        commonROMPermissionApply(context);
    }

    /**
     * Dp 2 px int.
     *
     * @param context the context
     * @param dp      the dp
     * @return the int
     */
    public static int dp2px(Context context, float dp) {
        final float scale = context.getResources().getDisplayMetrics().density;
        return (int) (dp * scale + 0.5f);
    }


    private static boolean huaweiPermissionCheck(Context context) {
        return HuaweiUtils.checkFloatWindowPermission(context);
    }

    private static boolean miuiPermissionCheck(Context context) {
        return MiuiUtils.checkFloatWindowPermission(context);
    }

    private static boolean meizuPermissionCheck(Context context) {
        return MeizuUtils.checkFloatWindowPermission(context);
    }

    private static boolean qikuPermissionCheck(Context context) {
        return QikuUtils.checkFloatWindowPermission(context);
    }

    private static boolean oppoROMPermissionCheck(Context context) {
        return OppoUtils.checkFloatWindowPermission(context);
    }

    private static boolean commonROMPermissionCheck(Context context) {
        // Recently found that this method doesn't work well on Meizu 6.0 system, only this one is special, need to adapt separately
        if (RomUtils.checkIsMeizuRom()) {
            return meizuPermissionCheck(context);
        } else {
            Boolean result = true;
            try {
                Class clazz = Settings.class;
                Method canDrawOverlays = clazz.getDeclaredMethod("canDrawOverlays", Context.class);
                result = (Boolean) canDrawOverlays.invoke(null, context);
            } catch (Exception e) {
                Log.e(TAG, Log.getStackTraceString(e));
            }
            return result;
        }
    }

    private static void rom360Permissionapply(final Context context) {
        showConfirmDialog(context, () -> QikuUtils.applyPermission(context));
    }

    private static void huaweiROMPermissionApply(final Context context) {
        showConfirmDialog(context, () -> HuaweiUtils.applyPermission(context));
    }

    private static void meizuROMPermissionApply(final Context context) {
        showConfirmDialog(context, () -> MeizuUtils.applyPermission(context, () -> {
            try {
                commonROMPermissionApplyInternal(context);
            } catch (NoSuchFieldException | IllegalAccessException e) {
                e.printStackTrace();
            }
        }));
    }

    private static void miuiROMPermissionApply(final Context context) {
        showConfirmDialog(context, () -> MiuiUtils.applyMiuiPermission(context));
    }

    private static void oppoROMPermissionApply(final Context context) {
        showConfirmDialog(context, () -> OppoUtils.applyOppoPermission(context));
    }

    /**
     * Common ROM permission application
     *
     * @param context Context.
     */
    private static void commonROMPermissionApply(final Context context) {
        // Same here, Meizu system needs to be adapted separately
        if (RomUtils.checkIsMeizuRom()) {
            meizuROMPermissionApply(context);
        } else {
            showConfirmDialog(context, () -> {
                try {
                    commonROMPermissionApplyInternal(context);
                } catch (Exception e) {
                    Log.e(TAG, Log.getStackTraceString(e));
                }
            });
        }
    }

    private static void commonROMPermissionApplyInternal(Context context) throws NoSuchFieldException, IllegalAccessException {
        Class clazz = Settings.class;
        Field field = clazz.getDeclaredField("ACTION_MANAGE_OVERLAY_PERMISSION");

        Intent intent = new Intent(field.get(null).toString());
        intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        intent.setData(Uri.parse("package:" + context.getPackageName()));
        context.startActivity(intent);
    }


    private static void showConfirmDialog(@NonNull Context context, final Runnable confirm) {
        new AlertDialog.Builder(context).setCancelable(true).setTitle("")
                .setMessage(R.string.float_window_confirm_dialog_msg)
                .setPositiveButton(R.string.float_window_confirm_dialog_confirm,
                        (dialog, which) -> {
                            if (confirm != null) {
                                confirm.run();
                            }
                            dialog.dismiss();
                        })
                .setNegativeButton(R.string.float_window_confirm_dialog_cancel,
                        (dialog, which) -> dialog.dismiss())
                .show();
    }
}
