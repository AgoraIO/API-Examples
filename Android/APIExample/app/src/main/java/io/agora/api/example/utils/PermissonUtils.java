package io.agora.api.example.utils;

import android.Manifest;
import android.content.Context;
import android.content.pm.PackageManager;
import android.os.Build;

import androidx.core.content.ContextCompat;

import java.util.ArrayList;
import java.util.List;

public class PermissonUtils {
    private static final String TAG = "PermissonUtils";

    public static String[] getCommonPermission() {
        List<String> permissionList = new ArrayList<>();
        permissionList.add(Manifest.permission.READ_EXTERNAL_STORAGE);
        permissionList.add(Manifest.permission.WRITE_EXTERNAL_STORAGE);
        permissionList.add(Manifest.permission.RECORD_AUDIO);
        permissionList.add(Manifest.permission.CAMERA);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            permissionList.add(Manifest.permission.READ_PHONE_STATE);
            permissionList.add(Manifest.permission.BLUETOOTH_CONNECT);
        }
        String[] permissionArray = new String[permissionList.size()];
        permissionList.toArray(permissionArray);
        return permissionArray;
    }

    //check array permission is granted
    public static boolean checkPermissions(Context context, String[] permissions) {
        for (String permission : permissions) {
            if (ContextCompat.checkSelfPermission(context, permission) != PackageManager.PERMISSION_GRANTED) {
                return false;
            }
        }
        return true;
    }

    //check single permission is granted
    public static boolean checkPermission(Context context, String permission) {
        return ContextCompat.checkSelfPermission(context, permission) == PackageManager.PERMISSION_GRANTED;
    }


    // Callback interface for permission results
    public interface PermissionResultCallback {
        void onPermissionsResult(boolean allPermissionsGranted, String[] permissions, int[] grantResults);
    }

}
