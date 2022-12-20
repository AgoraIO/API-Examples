package com.bytedance.labcv.common.utils;

import android.content.Context;
import android.content.SharedPreferences;

public class PreferenceUtils {

    private static final String CONFIG_FILE_NAME = "facegate_config";


    public static void setBoolFieldValue(Context context, String keyName,
                                  boolean value) {
        SharedPreferences sp = context.getSharedPreferences(CONFIG_FILE_NAME, Context.MODE_PRIVATE);
        SharedPreferences.Editor editor = sp.edit();
        editor.putBoolean(keyName, value);
        editor.commit();
    }

    public static boolean getBoolFieldValue(Context context, String keyName, boolean defaultVal){
        SharedPreferences sp = context.getSharedPreferences(CONFIG_FILE_NAME, Context.MODE_PRIVATE);
        return sp.getBoolean(keyName, defaultVal);
    }

    public static void setIntFieldValue(Context context, String keyName,
                                 int value) {
        SharedPreferences sp = context.getSharedPreferences(CONFIG_FILE_NAME, Context.MODE_PRIVATE);
        SharedPreferences.Editor editor = sp.edit();
        editor.putInt(keyName, value);
        editor.commit();
    }


    public static int getIntFieldValue(Context context, String keyName, int defValue) {
        SharedPreferences sp = context.getSharedPreferences(CONFIG_FILE_NAME, Context.MODE_PRIVATE);
        return sp.getInt(keyName, defValue);
    }

    public static void setFloatFieldValue(Context context, String keyName,
                                 float value) {
        SharedPreferences sp = context.getSharedPreferences(CONFIG_FILE_NAME, Context.MODE_PRIVATE);
        SharedPreferences.Editor editor = sp.edit();
        editor.putFloat(keyName, value);
        editor.commit();
    }


    public static float getFloatFieldValue(Context context, String keyName, float defValue) {
        SharedPreferences sp = context.getSharedPreferences(CONFIG_FILE_NAME, Context.MODE_PRIVATE);
        return sp.getFloat(keyName, defValue);
    }


    public static void setLongFieldValue(Context context, String keyName,
                                 long value) {
        SharedPreferences sp = context.getSharedPreferences(CONFIG_FILE_NAME, Context.MODE_PRIVATE);
        SharedPreferences.Editor editor = sp.edit();
        editor.putLong(keyName, value);
        editor.commit();
    }


    public static long getLongFieldValue(Context context, String keyName, long defValue) {
        SharedPreferences sp = context.getSharedPreferences(CONFIG_FILE_NAME, Context.MODE_PRIVATE);
        return sp.getLong(keyName, defValue);
    }


    public static void setStringFieldValue(Context context, String keyName, String value) {
        SharedPreferences sp = context.getSharedPreferences(CONFIG_FILE_NAME, Context.MODE_PRIVATE);
        SharedPreferences.Editor editor = sp.edit();
        editor.putString(keyName, value);
        editor.commit();
    }


    public static String getStringFieldValue(Context context, String keyName, String defValue) {
        SharedPreferences sp = context.getSharedPreferences(CONFIG_FILE_NAME, Context.MODE_PRIVATE);
        return sp.getString(keyName, defValue);
    }

}
