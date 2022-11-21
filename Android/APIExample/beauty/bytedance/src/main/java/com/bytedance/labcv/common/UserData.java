package com.bytedance.labcv.common;

import android.content.Context;
import android.content.SharedPreferences;

/**
 * Created on 2020/8/28 19:49
 */
public class UserData {
    public static final String NAME = "user";
    public static final String VERSION = "versionCode";
    public static final String RESOURCE_READY = "resource";

    private static volatile UserData sInstance;
    private SharedPreferences mSp;

    private UserData(Context context) {
        mSp = context.getSharedPreferences(NAME, Context.MODE_PRIVATE);
    }

    public static UserData getInstance(Context context) {
        if (sInstance == null) {
            synchronized (UserData.class) {
                if (sInstance == null) {
                    sInstance = new UserData(context);
                }
            }
        }
        return sInstance;
    }

    public int getVersion() {
        return mSp.getInt(VERSION, 0);
    }

    public void setVersion(int version) {
        mSp.edit().putInt(VERSION, version).apply();
    }

    public boolean isResourceReady() {
        return mSp.getBoolean(RESOURCE_READY, false);
    }

    public void setResourceReady(boolean ready) {
        mSp.edit().putBoolean(RESOURCE_READY, ready).apply();
    }
}
