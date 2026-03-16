package io.agora.api.example.compose.utils;

import android.text.TextUtils;

import io.agora.api.example.compose.BuildConfig;

public final class AgoraConfig {
    private AgoraConfig() {
    }

    public static String getAppId() {
        return BuildConfig.AGORA_APP_ID;
    }

    public static String getAppCertificate() {
        return BuildConfig.AGORA_APP_CERT;
    }

    public static boolean hasAppCertificate() {
        return !TextUtils.isEmpty(getAppCertificate());
    }
}
