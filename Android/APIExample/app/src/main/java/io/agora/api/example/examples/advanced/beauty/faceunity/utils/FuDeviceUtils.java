package io.agora.api.example.examples.advanced.beauty.faceunity.utils;

import static android.content.Context.MODE_PRIVATE;

import android.content.Context;
import android.content.SharedPreferences;
import android.os.Build;
import android.util.Log;

import java.io.File;

import io.agora.api.example.examples.advanced.beauty.faceunity.utils.device.DeviceScoreUtils;


public class FuDeviceUtils {

    public static final String TAG = "FuDeviceUtils";

    public static final String DEVICE_LEVEL = "device_level";

    public static final int DEVICE_LEVEL_FOUR = 4;
    public static final int DEVICE_LEVEL_THREE = 3;

    public static final int DEVICE_LEVEL_TWO = 2;
    public static final int DEVICE_LEVEL_ONE = 1;
    public static final int DEVICE_LEVEL_MINUS_ONE = -1;
    public static final int DEVICE_LEVEL_EMPTY = -100;

    public static final double DEVICE_SCORE_FOUR = 85.0;
    public static final double DEVICE_SCORE_THREE = 78.0;
    public static final double DEVICE_SCORE_TWO = 69.0;
    public static final double DEVICE_SCORE_ONE = 64.0;

    public static final String[] levelFourDevices = {};
    public static final String[] levelThreeDevices = {};
    public static final String[] levelTwoDevices = {};
    public static final String[] levelOneDevices = {"PRO 7 Plus"};
    public static final String Nexus_6P = "Nexus 6P";


    public static int judgeDeviceLevel(Context context) {
        return judgeDeviceLevel(context,false);
    }

    public static int judgeDeviceLevel(Context context,boolean ignoreCache) {
        if (!ignoreCache) {
            int cacheDeviceLevel = getCacheDeviceLevel(context);
            if (cacheDeviceLevel > DEVICE_LEVEL_EMPTY) {
                return cacheDeviceLevel;
            }
        }
        //有一些设备不符合下述的判断规则，则走一个机型判断模式
        int specialDevice = judgeDeviceLevelInDeviceName();
        if (specialDevice > DEVICE_LEVEL_EMPTY) {
            return specialDevice;
        }
        double score = DeviceScoreUtils.getDeviceScore();
        int level = DEVICE_LEVEL_MINUS_ONE;
        if (score > DEVICE_SCORE_ONE) {
            level = DEVICE_LEVEL_ONE;
        }
        if (score > DEVICE_SCORE_TWO) {
            level = DEVICE_LEVEL_TWO;
        }
        if (score > DEVICE_SCORE_THREE) {
            level = DEVICE_LEVEL_THREE;
        }
        if (score > DEVICE_SCORE_FOUR) {
            level = DEVICE_LEVEL_FOUR;
        }
        saveCacheDeviceLevel(context,level);
        Log.d(TAG, "CPUName: " + DeviceScoreUtils.CPUName + " GPUName: " + DeviceScoreUtils.GPUName + " score: " + score + " level: " + level);
        return level;
    }

    private static int judgeDeviceLevelInDeviceName() {
        String currentDeviceName = getDeviceName();
        for (String deviceName : levelFourDevices) {
            if (deviceName.equals(currentDeviceName)) {
                return DEVICE_LEVEL_FOUR;
            }
        }
        for (String deviceName : levelThreeDevices) {
            if (deviceName.equals(currentDeviceName)) {
                return DEVICE_LEVEL_THREE;
            }
        }
        for (String deviceName : levelTwoDevices) {
            if (deviceName.equals(currentDeviceName)) {
                return DEVICE_LEVEL_TWO;
            }
        }
        for (String deviceName : levelOneDevices) {
            if (deviceName.equals(currentDeviceName)) {
                return DEVICE_LEVEL_ONE;
            }
        }
        return DEVICE_LEVEL_EMPTY;
    }

    public static String getDeviceName() {
        String deviceName = "";
        if (Build.MODEL != null) {
            deviceName = Build.MODEL;
        }
        Log.d(TAG, "deviceName: " + deviceName);
        return deviceName;
    }

    public static void saveCacheDeviceLevel(Context context,int level) {
        SharedPreferences sp = context.getSharedPreferences(DEVICE_LEVEL, MODE_PRIVATE);
        sp.edit().putInt(DEVICE_LEVEL, level).apply();
    }

    public static int getCacheDeviceLevel(Context context) {
        SharedPreferences sp = context.getSharedPreferences(DEVICE_LEVEL, MODE_PRIVATE);
        return sp.getInt(DEVICE_LEVEL, DEVICE_LEVEL_EMPTY);
    }

    public static class Config{
        public static String BUNDLE_AI_FACE = "model" + File.separator + "ai_face_processor.bundle";
        public static String BUNDLE_AI_HAND = "model" + File.separator + "ai_hand_processor.bundle";
        public static String BUNDLE_AI_HUMAN = "model" + File.separator + "ai_human_processor.bundle";
        public static String BUNDLE_AI_TONGUE = "graphics" + File.separator + "tongue.bundle";
        public static int DEVICE_LEVEL = FuDeviceUtils.DEVICE_LEVEL_TWO;
        public static String DEVICE_NAME = "";
        public static final boolean FACE_DELAY_LEAVE_ENABLE = false;
    }
}
