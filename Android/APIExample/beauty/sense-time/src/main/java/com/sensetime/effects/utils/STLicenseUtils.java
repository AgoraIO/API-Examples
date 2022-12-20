package com.sensetime.effects.utils;


import android.content.Context;
import android.content.SharedPreferences;
import android.text.TextUtils;
import android.util.Log;

import com.sensetime.sensearsourcemanager.SenseArMaterialService;
import com.sensetime.stmobile.STMobileAuthentificationNative;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.Locale;

/**
 * Created by leiwang on 2016/12/2.
 */

public class STLicenseUtils {
    private final static String TAG = "STLicenseUtils";


    //是否使用服务器License鉴权
    //true：使用服务器下拉授权文件，使用离线接口生成activeCode
    //false: 使用asset文件夹下的 "SenseME.lic"，"SenseME_Online.lic"生成activeCode
    private static final boolean USING_SERVER_LICENSE = false;

    //是否使用在线鉴权接口，使用SenseME_Online.lic文件
    //true: 使用asset文件夹下的"SenseME_Online.lic"，使用在线接口generateActiveCodeFromBufferOnline生成activeCode
    //false ：使用asset文件夹下的"SenseME.lic"，使用离线接口generateActiveCodeFromBuffer生成activeCode
    private static final boolean USING_ASSETS_ONLINE_LICENSE = false;

    private final static String PREF_ACTIVATE_CODE_FILE = "activate_code_file";
    private final static String PREF_ACTIVATE_CODE = "activate_code";

    private static final String LOCAL_LICENSE_NAME = "license/SenseME.lic";//离线generateActiveCode使用
    private static final String ONLINE_LICENSE_NAME = "license/SenseME_Online.lic";//在线generateActiveCode使用

    private static volatile boolean IsChecked = false;
    private static boolean mCheckLicenseRet = false;

    //鉴权方式有两种
    public static boolean checkLicense(final Context context){
        if(USING_SERVER_LICENSE){
            return checkLicenseFromServer(context);
        }else{
            return checkLicenseFromLocal(context);
        }
    }

    /**
    * 检查activeCode合法性
    * @return true, 成功 false,失败
    */
    public static boolean checkLicenseFromAssetFile(Context context, String fileName, boolean isOnlineLicense) {
        StringBuilder sb = new StringBuilder();
        InputStreamReader isr = null;
        BufferedReader br = null;
        // 读取license文件内容
        try {
            isr = new InputStreamReader(context.getResources().getAssets().open(fileName));
            br = new BufferedReader(isr);
            String line = null;
            while((line=br.readLine()) != null) {
                sb.append(line).append("\n");
            }
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
            if (isr != null) {
                try {
                    isr.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }

            if (br != null) {
                try {
                    br.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }

        // license文件为空,则直接返回
        if (sb.toString().length() == 0) {
            LogUtils.e(TAG, "read license data error");
            return false;
        }

        String licenseBuffer = sb.toString();
        /**
         * 以下逻辑为：
         * 1. 获取本地保存的激活码
         * 2. 如果没有则生成一个激活码
         * 3. 如果有, 则直接调用checkActiveCode*检查激活码
         * 4. 如果检查失败，则重新生成一个activeCode
         * 5. 如果生成失败，则返回失败，成功则保存新的activeCode，并返回成功
         */
        SharedPreferences sp = context.getApplicationContext().getSharedPreferences(PREF_ACTIVATE_CODE_FILE, Context.MODE_PRIVATE);
        String activateCode = sp.getString(PREF_ACTIVATE_CODE, null);
        Integer error = new Integer(-1);
        if (activateCode == null|| (STMobileAuthentificationNative.checkActiveCodeFromBuffer(context, licenseBuffer, licenseBuffer.length(), activateCode, activateCode.length()) != 0)) {
            LogUtils.e(TAG, "activeCode: " + (activateCode == null));
            if(isOnlineLicense){
                activateCode = STMobileAuthentificationNative.generateActiveCodeFromBufferOnline(context, licenseBuffer, licenseBuffer.length());
            }else{
                activateCode = STMobileAuthentificationNative.generateActiveCodeFromBuffer(context, licenseBuffer, licenseBuffer.length());
            }
            if (activateCode != null && activateCode.length() >0) {
                SharedPreferences.Editor editor = sp.edit();
                editor.putString(PREF_ACTIVATE_CODE, activateCode);
                editor.commit();
                return true;
            }
            LogUtils.e(TAG, "generate license error: " + error);
            return false;
        }

        LogUtils.e(TAG, "activeCode: " + activateCode);

        return true;
    }

    /**
     * 根据传入lic buffer检查activeCode合法性
     * @param context
     * @param licBuffer
     * @return
     */
    public static boolean checkLicenseFromBuffer(Context context, byte[] licBuffer, boolean isOnlineLicense){
        if(licBuffer == null){
            Log.e(TAG, "checkLicenseFromBuffer: licBuffer is null ");
            return false;
        }

        String licenseBuffer = new String(licBuffer);
        /**
         * 以下逻辑为：
         * 1. 获取本地保存的激活码
         * 2. 如果没有则生成一个激活码
         * 3. 如果有, 则直接调用checkActiveCode*检查激活码
         * 4. 如果检查失败，则重新生成一个activeCode
         * 5. 如果生成失败，则返回失败，成功则保存新的activeCode，并返回成功
         */
        SharedPreferences sp = context.getApplicationContext().getSharedPreferences(PREF_ACTIVATE_CODE_FILE, Context.MODE_PRIVATE);
        String activateCode = sp.getString(PREF_ACTIVATE_CODE, null);
        Integer error = new Integer(-1);
        if (activateCode == null|| (STMobileAuthentificationNative.checkActiveCodeFromBuffer(context, licenseBuffer, licenseBuffer.length(), activateCode, activateCode.length()) != 0)) {
            LogUtils.e(TAG, "activeCode: " + (activateCode == null));
            if(isOnlineLicense){
                activateCode = STMobileAuthentificationNative.generateActiveCodeFromBufferOnline(context, licenseBuffer, licenseBuffer.length());
            }else{
                activateCode = STMobileAuthentificationNative.generateActiveCodeFromBuffer(context, licenseBuffer, licenseBuffer.length());
            }
            if (activateCode != null && activateCode.length() >0) {
                SharedPreferences.Editor editor = sp.edit();
                editor.putString(PREF_ACTIVATE_CODE, activateCode);
                editor.commit();
                return true;
            }
            LogUtils.e(TAG, "generate license error: " + error);
            return false;
        }

        LogUtils.e(TAG, "activeCode: " + activateCode);

        return true;
    }

    //从服务器拉取License文件，并检查授权
    public static boolean checkLicenseFromServer(final Context context){
        if (!NetworkUtils.isNetworkAvailable(context)) {
            LogUtils.d(TAG, "无网情况下，拉取sdk缓存License文件，激活License授权");
            byte[] licData = SenseArMaterialService.shareInstance().getLicenseData();

            if(licData != null){
                Log.e(TAG, "no network : "+ new String(licData) );
            }else {
                Log.e(TAG, "no network licData is null " );
            }


            return STLicenseUtils.checkLicenseFromBuffer(context, licData, false);
        }

        SenseArMaterialService.shareInstance().authorizeWithAppId(context, Constants.APPID, Constants.APPKEY, new SenseArMaterialService.OnAuthorizedListener() {
            @Override
            public void onSuccess() {
                Log.d(TAG, "current Thread" + Thread.currentThread().getName());
                LogUtils.d(TAG, "鉴权成功！");
                //鉴权成功后，可以获取远程的lic 文件数据,用来checkLicense
                byte[] licData = SenseArMaterialService.shareInstance().getLicenseData();

                if(licData != null){
                    Log.e(TAG, "onSuccess: "+new String(licData) );
                }else {
                    Log.e(TAG, "onSuccess: licData is null " );
                }

                mCheckLicenseRet = STLicenseUtils.checkLicenseFromBuffer(context, licData, false);
                IsChecked = true;
            }

            @Override
            public void onFailure(SenseArMaterialService.AuthorizeErrorCode errorCode, String errorMsg) {
                LogUtils.d(TAG, String.format(Locale.getDefault(), "鉴权失败！%d, %s", errorCode.ordinal(), TextUtils.isEmpty(errorMsg)?"":errorMsg));

                //鉴权失败后，可以获取缓存的lic 文件数据,用来checkLicense
                byte[] licData = SenseArMaterialService.shareInstance().getLicenseData();
                if(licData != null){
                    Log.e(TAG, "onFailure: "+new String(licData) );
                }else {
                    Log.e(TAG, "onFailure: licData is null " );
                }

                mCheckLicenseRet = STLicenseUtils.checkLicenseFromBuffer(context, licData, false);
                IsChecked = true;
            }
        });

        while(!IsChecked){
            try {
                Thread.sleep(100);
            }catch (Exception e){
                Log.e(TAG, "sleep Exception: " + e.getMessage() );
            }
        }

        return mCheckLicenseRet;
    }

    //从本地读取License文件，并检查授权
    public static boolean checkLicenseFromLocal(final Context context){
        if(USING_ASSETS_ONLINE_LICENSE){
            if (!STLicenseUtils.checkLicenseFromAssetFile(context, ONLINE_LICENSE_NAME, true)) {
                return false;
            }
        }else {
            if (!STLicenseUtils.checkLicenseFromAssetFile(context, LOCAL_LICENSE_NAME, false)) {
                return false;
            }
        }


        return true;
    }
}
