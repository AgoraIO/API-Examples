// Copyright (C) 2018 Beijing Bytedance Network Technology Co., Ltd.
package io.agora.extension;

import android.content.Context;
import android.content.SharedPreferences;

import androidx.annotation.NonNull;

import java.io.File;

public class ResourceHelper {

    public static final String RESOURCE = "resource";
    public static final String FILTER_RESOURCE = "FilterResource.bundle/Filter";

    public static final String FACE = "ModelResource.bundle/ttfacemodel/tt_face_v8.2.model";
    public static final String PETFACE = "ModelResource.bundle/ttpetface/tt_petface_v2.4.model";
    public static final String DetectParamFile = "ModelResource.bundle/handmodel/tt_hand_det_v9.0.model";
    public static final String BoxRegParamFile = "ModelResource.bundle/handmodel/tt_hand_box_reg_v10.0.model";
    public static final String GestureParamFile = "ModelResource.bundle/handmodel/tt_hand_gesture_v8.1.model";
    public static final String KeyPointParamFile = "ModelResource.bundle/handmodel/tt_hand_kp_v5.0.model";
    public static final String SegParamFile = "ModelResource.bundle/handmodel/tt_hand_seg_v1.0.model";

    private static final String FACEEXTA = "ModelResource.bundle/ttfacemodel/tt_face_extra_v11.0.model";
    private static final String FACEATTRI = "ModelResource.bundle/ttfaceattrmodel/tt_face_attribute_v6.1.model";
    private static final String FACEVERIFY = "ModelResource.bundle/ttfaceverify/tt_faceverify_v5.0.model";
    private static final String SKELETON = "ModelResource.bundle/skeleton_model/tt_skeleton_v6.0.model";
    private static final String PORTRAITMATTING = "ModelResource.bundle/mattingmodel/tt_matting_v9.0.model";
    private static final String HEADSEGMENT = "ModelResource.bundle/headseg/tt_headseg_v5.0.model";
    private static final String HAIRPARSING = "ModelResource.bundle/hairparser/tt_hair_v9.0.model";
//    private static final String LIGHTCLS = "ModelResource.bundle/lightcls/tt_lightcls_v1.0.model";
    private static final String HUMANDIST = "ModelResource.bundle/humandistance/tt_humandist_v1.0.model";
    public static final String GENERAL_OBJECT_DETECT = "ModelResource.bundle/generalobjectmodel/tt_general_obj_detection_v1.0.model";
    public static final String GENERAL_OBJECT_CLS = "ModelResource.bundle/generalobjectmodel/tt_general_obj_detection_cls_v1.0.model";
    public static final String GENERAL_OBJECT_TRACK = "ModelResource.bundle/generalobjectmodel/tt_sample_v1.0.model";

    private static final String LICENSE_NAME = "Agora_20200915_20210914_agoramarketplace.bytedance.labcv.demo_v3.9.3.1.licbag";

    private static String getResourcePath(Context context) {
        return context.getExternalFilesDir("assets").getAbsolutePath() + File.separator + RESOURCE;
    }

    public static String getModelDir(@NonNull final Context context) {
        File file = new File(new File(getResourcePath(context), "ModelResource.bundle"), "");
        return file.getAbsolutePath();
    }

    public static String getLicensePath(@NonNull final Context context) {
        File file = new File(new File(getResourcePath(context), "LicenseBag.bundle"), LICENSE_NAME);
        return file.getAbsolutePath();
    }

    public static String getStickersPath(@NonNull final Context context) {
        File file = new File(new File(getResourcePath(context), "StickerResource.bundle"), "stickers");
        return file.getAbsolutePath();
    }

    public static String getAnimojiPath(final Context context) {
        File file = new File(new File(getResourcePath(context), "StickerResource.bundle"), "animoji");
        return file.getAbsolutePath();
    }

    public static String getGamePath(Context context) {
        File file = new File(new File(getResourcePath(context), "StickerResource.bundle"), "game");
        return file.getAbsolutePath();
    }

    public static String getComposeMakeupComposerPath(@NonNull final Context context) {
        return getResourcePath(context) + File.separator + "ComposeMakeup.bundle" + File.separator + "ComposeMakeup/composer";
    }

    public static String getComposePath(@NonNull final Context context) {
        return getResourcePath(context) + File.separator + "ComposeMakeup.bundle"  + File.separator + "ComposeMakeup/";
    }


    public static File[] getFilterResources(@NonNull final Context context) {
        return getResources(context, FILTER_RESOURCE);
    }

    public static String getFaceModelPath(@NonNull final Context context) {
        File file = new File(getResourcePath(context), FACE);
        return file.getAbsolutePath();
    }

    public static String getPetFaceModelPath(@NonNull final Context context) {
        File file = new File(new File(getResourcePath(context), PETFACE), "");
        return file.getAbsolutePath();
    }

    public static String getFaceExtaModelPath(@NonNull final Context context) {
        File file = new File(new File(getResourcePath(context), FACEEXTA), "");
        return file.getAbsolutePath();
    }

    public static String getFaceAttriModelPath(@NonNull final Context context) {
        File file = new File(new File(getResourcePath(context), FACEATTRI), "");
        return file.getAbsolutePath();
    }

    public static String getFaceVerifyModelPath(final Context context) {
        File file = new File(new File(getResourcePath(context), FACEVERIFY), "");
        return file.getAbsolutePath();
    }

    public static String getSkeletonModelPath(@NonNull final Context context) {
        File file = new File(new File(getResourcePath(context), SKELETON), "");
        return file.getAbsolutePath();
    }

    public static String getPortraitmattingModelPath(@NonNull final Context context)
    {
        File file = new File(new File(getResourcePath(context), PORTRAITMATTING), "");
        return file.getAbsolutePath();
    }

    public static String getHeadSegmentModelPath(@NonNull final Context context)
    {
        File file = new File(new File(getResourcePath(context), HEADSEGMENT), "");
        return file.getAbsolutePath();
    }


    public static String getHairParsingModelPath(@NonNull final Context context)
    {
        File file = new File(new File(getResourcePath(context), HAIRPARSING), "");
        return file.getAbsolutePath();
    }

    public static String getHandModelPath(@NonNull final Context context,String path) {
        File file = new File(new File(getResourcePath(context), path), "");
        return file.getAbsolutePath();
    }

//    public static String getLightClsModelPath(final Context context) {
//        File file = new File(getResourcePath(context), LIGHTCLS);
//        return file.getAbsolutePath();
//    }

    public static String getGeneralObjectDetect(final Context context) {
        return new File(getResourcePath(context), GENERAL_OBJECT_DETECT).getAbsolutePath();
    }

    public static String getGeneralObjectCls(final Context context) {
        return new File(getResourcePath(context), GENERAL_OBJECT_CLS).getAbsolutePath();
    }

    public static String getGeneralObjectTrack(final Context context) {
        return new File(getResourcePath(context), GENERAL_OBJECT_TRACK).getAbsolutePath();
    }

    public static String getHumandistModelPath(final Context context) {
        File file = new File(getResourcePath(context), HUMANDIST);
        return file.getAbsolutePath();
    }



    public static File[] getResources(@NonNull final Context context, String type) {
        File file = new File(new File(getResourcePath(context), type), "");
        if (file.exists() && file.isDirectory())
            return file.listFiles();
        return new File[0];
    }

    public static String getStickerPath(@NonNull final Context context,String name) {
        return getStickersPath(context) + File.separator + name;

    }

    public static  String getAnimojiPath(final Context context, String name) {
        return getAnimojiPath(context) + File.separator + name;
    }

    public static String getGamePath(Context context, String name) {
        return getGamePath(context) + File.separator + name;
    }

    public static boolean isResourceReady(@NonNull final Context context, int versionCode) {

        SharedPreferences preferences=context.getSharedPreferences("user", Context.MODE_PRIVATE);
        boolean resourceReady = preferences.getBoolean("resource", false);
        int preVersioncode = preferences.getInt("versionCode", 0);

        // 如果上次已经拷贝过 继续检查版本号
        // Continue to check the version number if it was copied last time
        if (resourceReady && versionCode == preVersioncode){
            return true;
        }
        return false;
    }

    public static void setResourceReady(@NonNull final Context context, boolean isReady, int versionCode){
        SharedPreferences preferences=context.getSharedPreferences("user", Context.MODE_PRIVATE);

        SharedPreferences.Editor editor = preferences.edit();
        editor.putBoolean("resource", isReady);
        editor.putInt("versionCode", versionCode);
        editor.commit();
    }

    public static String getDownloadedStickerDir(@NonNull final Context context){
        File file = new File(new File(getResourcePath(context), "download"), "sticker");
        if (!file.exists()) {
            file.mkdirs();

        }
        return file.getAbsolutePath();


    }

    public static String getFilterResourcePathByName(@NonNull final Context context, String filterName){
        File file = new File(new File(getResourcePath(context), FILTER_RESOURCE), "");

        return file.getAbsolutePath() + File.separator + filterName;
    }




}
