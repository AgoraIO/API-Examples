package io.agora.beautyapi.sensetime.utils.utils;

public class Constants {
    public static final String SP_FIRST = "sp_first";

    public static final String CFG_K_OPEN_ADD_STICKER = "openAddSticker";
    public static final String CFG_K_OPEN_OVERLAP_DEBUG = "openOverlapDebug";
    public static final String CFG_K_LOCAL_TRY_ON_DATA = "localTryOnData";
    public static final String CFG_K_DRAW_POINTS = "drawPoints";
    public static final String CFG_K_GO_FACE_SHAPE = "scanFaceShape";
    public static final String CFG_K_ZEROBTN = "zeroBtn";

    public static final String ASSET_PATH_HTML = "html";
    public static final String ASSET_PATH_TERMS = "file:///android_asset/html/SenseME_Provisions_v1.0.html";

    // 美白2对应的素材
    public static final String WHITENING_PATH = "whiten_gif.zip";

    public static final String STICKER_SYNC = "stickerSync";
    public static final String STICKER_LOCAL = "newEngine";// 本地贴纸文件夹

    // TryOn本地素材包
    public static final String TRY_ON_ASSET_PATH = "tryOn";

    public static final String TRY_ON_LIP = TRY_ON_ASSET_PATH + "/lip";//口红
    public static final String TRY_ON_HAIR = TRY_ON_ASSET_PATH + "/hair";//染发
    public static final String TRY_ON_LIP_LINE = TRY_ON_ASSET_PATH + "/lipline";// 唇线
    public static final String TRY_ON_EYE_SHADOW = TRY_ON_ASSET_PATH + "/eyeShadow";//眼影
    public static final String TRY_ON_EYELINER = TRY_ON_ASSET_PATH + "/eyeliner";//眼线

    public static final String TRY_ON_EYE_PRINT = TRY_ON_ASSET_PATH + "/eyePrint";//眼印(stampliner.zip)
    public static final String TRY_ON_EYE_LASH = TRY_ON_ASSET_PATH + "/eyeLash";//眼睫毛
    public static final String TRY_ON_EYE_BROW = TRY_ON_ASSET_PATH + "/eyeBrow";//眉毛
    public static final String TRY_ON_BLUSH = TRY_ON_ASSET_PATH + "/blush";//腮红
    public static final String TRY_ON_TRIMMING = TRY_ON_ASSET_PATH + "/trimming";//修容(contour)

    public static final String TRY_ON_FOUNDATION = TRY_ON_ASSET_PATH + "/foundation";//粉底

    public static final String CHOOSE_BG_STICKER_ID = "20211221152305770599802";

    public static final int ATY_TYPE_CAMERA = 0;
    public static final int ATY_TYPE_TRY_ON = 1;
    public static final int ATY_TYPE_VIDEO = 2;
    public static final int ATY_TYPE_IMAGE = 3;

    // 重启恢复效果用，标识
    public static final int SP_WHITEN_1 = 0;
    public static final int SP_WHITEN_2 = 1;
    public static final int SP_WHITEN_3 = 2;

    // 基础美颜
    public static final float[] mNewBeautifyParamsTypeBase = {
            0.00f,  // 1.美白1
            0.00f,  // 2.美白2
            0.00f,  // 3.美白3
            0.00f,  // 4.红润
            0.00f,  // 5.磨皮1
            0.50f   // 6.磨皮2
    };
    // 美形
    public static final float[] mNewBeautifyParamsTypeProfessional = {
            0.34f,  // 1.瘦脸
            0.29f,  // 2.大眼
            0.10f,  // 3.小脸
            0.25f,  // 4.窄脸
            0.07f   // 5.圆眼
    };
    // 微整形
    public static final float[] mNewBeautifyParamsTypeMicro = {
            0.00f,  // 1.小头
            0.45f,  // 2.瘦脸型
            0.20f,  // 3.下巴
            0.00f,  // 4.额头
            0.30f,  // 5.苹果肌
            0.21f,   // 6.瘦鼻翼
            0.00f,  // 7.长鼻
            0.10f,  // 8.侧脸隆鼻
            0.51f,  // 9.嘴型
            0.00f,  // 10.缩人中
            -0.23f, // 11.眼距
            0.00f,  // 12.眼睛角度
            0.00f,  // 13.开眼角
            0.25f,  // 14.亮眼
            0.69f,  // 15.祛黑眼圈
            0.60f,  // 16.祛法令纹
            0.20f,  // 17.白牙
            0.36f,  // 18.瘦颧骨
            0.00f   // 19.开外眼角
    };
    // 调整
    public static final float[] mNewBeautifyParamsTypeAdjust = {
            0.00f,  // 1.对比度
            0.00f,  // 2.饱和度
            0.50f,  // 3.清晰度
            0.20f   // 4.锐化
    };

    public static boolean ACTIVITY_MODE_LANDSCAPE = false;
    public static boolean ACTIVITY_MODE_FOR_TV = false;

    //正式服appid appkey
    public final static String APPID = "6dc0af51b69247d0af4b0a676e11b5ee";//正式服
    public final static String APPKEY = "e4156e4d61b040d2bcbf896c798d06e3";//正式服

    //正式服group id
    // ↓↓↓↓↓↓↓↓↓ sticker
    public final static String GROUP_NEW = "ff81fc70f6c111e899f602f2be7c2171";//最新
    public final static String GROUP_2D = "3cd2dae0f6c211e8877702f2beb67403";// 2d
    public final static String GROUP_3D = "4e869010f6c211e888ea020d88863a42";// 3D
    public final static String GROUP_HAND = "5aea6840f6c211e899f602f2be7c2171";// 手势
    public final static String GROUP_BG = "65365cf0f6c211e8877702f2beb67403";// 背景

    public final static String GROUP_FACE = "6d036ef0f6c211e899f602f2be7c2171";//变形
    public final static String GROUP_PARTICLE = "7c6089f0f6c211e8877702f2beb67403";//粒子
    public final static String GROUP_CAT = "f101913d44fb42f2ad279a9b383062c8";//猫脸
    public final static String GROUP_BIG_HEAD = "36d0ec16b7684703a82bb59b0b0f7f4e";//大头
    public final static String GROUP_FENSHEN = "1ae998ea4dc8489da76346df0daff8ca";//影分身

    public final static String GROUP_MASK_FACE = "caa93c1160e2440eb8dbb4c9e42fa961";//扣脸
    public final static String GROUP_PLAY = "837faa0485a7462b982d9709aa124b4f";//特效玩法
    public final static String GROUP_AVATAR = "46028a20f6c211e888ea020d88863a42";//Avatar
    public final static String GROUP_TRY_ON = "1d494ab522ed4866862b17561b7e6bc6";//TryOn
    // ↑↑↑↑↑↑↑↑ sticker


    public static final int MAKEUP_TYPE_COUNT = 11;
}
