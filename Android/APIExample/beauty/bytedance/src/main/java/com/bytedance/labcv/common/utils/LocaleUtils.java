package com.bytedance.labcv.common.utils;

import android.content.Context;
import android.os.Build;

import com.bytedance.labcv.core.util.LogUtils;

import java.util.HashSet;
import java.util.Locale;
import java.util.Set;

public class LocaleUtils {

    public static String systemLanguage;

    public static Set<String> FACE_ALLOW =new HashSet<>();
    static {
        FACE_ALLOW.add("zh-CN");
    }

    public static Set<String> CAR_BRAND_ALLOW =new HashSet<>();
    static {
        CAR_BRAND_ALLOW.add("zh-CN");
    }

    public static Set<String> ASIA_LOCALES =new HashSet<>();
    static {
        ASIA_LOCALES.add("km-KH");
        ASIA_LOCALES.add("lo-LA");
        ASIA_LOCALES.add("fil-PH");

        ASIA_LOCALES.add("zh-CN");
        ASIA_LOCALES.add("zh-HK");
        ASIA_LOCALES.add("zh-MO");
        ASIA_LOCALES.add("zh-SG");
        ASIA_LOCALES.add("zh-TW");

        ASIA_LOCALES.add("en-MY");
        ASIA_LOCALES.add("en-SG");
        ASIA_LOCALES.add("en-PH");
        ASIA_LOCALES.add("in-ID");
        ASIA_LOCALES.add("ja-JP");
        ASIA_LOCALES.add("ko-KR");
        ASIA_LOCALES.add("ms-MY");
        ASIA_LOCALES.add("ms-BN");
        ASIA_LOCALES.add("th-TH");
        ASIA_LOCALES.add("vi-VN");


    }





    public static Locale getCurrentLocale(Context context){
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N){
            return context.getResources().getConfiguration().getLocales().get(0);
        } else{
            return context.getResources().getConfiguration().locale;
        }
    }

    private static String getLang(Context context){
        Locale locale = getCurrentLocale(context);
        return locale.getLanguage() + "-" + locale.getCountry();


    }

    public static boolean isFaceLimit(Context context){
        LogUtils.d("current language ="+getLang(context));
        return !FACE_ALLOW.contains(getLang(context));


    }
    public static boolean isCarBrandLimit(Context context){
        LogUtils.d("current language ="+getLang(context));
        return !CAR_BRAND_ALLOW.contains(getLang(context));


    }

    public static boolean isAsia(Context context){
        LogUtils.d("current language ="+getLang(context));
        return ASIA_LOCALES.contains(getLang(context));

    }

    public static String getLanguage(Context context) {
        systemLanguage = getCurrentLocale(context).getLanguage();
        return systemLanguage;
    }


}
