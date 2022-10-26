package com.byteddance.effect;

import static com.bytedance.labcv.effectsdk.BytedEffectConstants.BytedResultCode.BEF_RESULT_SUC;

import android.app.ActivityManager;
import android.content.Context;
import android.content.pm.ConfigurationInfo;
import android.opengl.GLES11Ext;
import android.text.TextUtils;
import android.util.Log;

import com.bytedance.labcv.effectsdk.BytedEffectConstants;
import com.bytedance.labcv.effectsdk.RenderManager;
import com.byteddance.model.ComposerNode;
import com.byteddance.model.TextureFormat;

import java.util.HashSet;
import java.util.Set;

public class EffectHelper {
    public static final String TAG = "EffectHelper";

    private volatile boolean mInitialized = false;
    private RenderManager mRenderManager;
    private EffectRender mEffectRender;
    private String mFilterResource;
    private String[] mComposeNodes = new String[0];
    private String mStickerResource;
    private Set<ComposerNode> mSavedComposerNodes = new HashSet<>();
    private float mFilterIntensity = 0f;
    private boolean mIsEffectOn = true;
    private Context mContext;

    public EffectHelper(Context context) {
        mContext = context;
        mRenderManager = new RenderManager();
    }

    /**
     * 初始化特效SDK，确保在gl线程中执行
     */
    public boolean initEffectSDK() {
        if (mInitialized) return true;
        mInitialized = initEffect(mContext) == BEF_RESULT_SUC;
        return mInitialized;
    }

    /**
     * 特效初始化入口
     *
     * @param context     应用上下文
     * @return 如果成功返回BEF_RESULT_SUC， 否则返回对应的错误码
     */
    private int initEffect(Context context) {
        ResourceHelper.initialize(context);
        String modelDir = ResourceHelper.getModelDir(context);
        String licence = ResourceHelper.getLicensePath(context);
        mEffectRender = new EffectRender();

        ActivityManager am = (ActivityManager) context.getSystemService(mContext.ACTIVITY_SERVICE);
        ConfigurationInfo ci = am.getDeviceConfigurationInfo();
        int renderapi = (ci.reqGlEsVersion >= 0x30000)?1:0;
        int ret = mRenderManager.init(context, modelDir, licence, true, false, renderapi);
        if (ret != BEF_RESULT_SUC) {
            Log.e(TAG, "mRenderManager.init failed!! ret =" + ret);
        }
        return ret;
    }

    public boolean initialized() {
        return mInitialized;
    }

    /**
     * 工作在渲染线程
     * Work on the render thread
     */
    public void destroyEffectSDK() {
        mRenderManager.release();
        mEffectRender.release();
        mInitialized = false;
    }

    public void setEffectOn(boolean isOn) {
        mIsEffectOn = isOn;
    }

    /**
     * 开启或者关闭滤镜 如果path为空 关闭滤镜
     * Turn filters on or off
     * turn off filter if path is empty
     *
     * @param path path of filter file 滤镜资源文件路径
     */
    public boolean setFilter(String path) {
        mFilterResource = path;
        return mRenderManager.setFilter(path);
    }

    /**
     * 设置特效组合，目前支持美颜、美形、美体、 美妆特效的任意叠加
     * Set special effects combination
     * Currently only support the arbitrary superposition of two special effects, beauty and beauty makeup
     *
     * @param nodes
     * @return
     */
    public boolean setComposeNodes(String[] nodes) {
        // clear mSavedComposerNodes cache when nodes length is 0
        if (nodes.length == 0) {
            mSavedComposerNodes.clear();
        }

        mComposeNodes = nodes;
        String prefix = ResourceHelper.getComposePath(mContext);
        String[] path = new String[nodes.length];
        for (int i = 0; i < nodes.length; i++) {
            path[i] = prefix + nodes[i];
        }
        return mRenderManager.setComposerNodes(path) == BEF_RESULT_SUC;
    }

    /**
     * 更新组合特效(美颜、美形、美体、 美妆)中某个节点的强度
     * Updates the strength of a node in a composite effect
     *
     * @param node The ComposerNode corresponding to the special effects material
     *             特效素材对应的 ComposerNode
     * @return
     */
    public boolean updateComposeNode(ComposerNode node, boolean update) {
        if (update) {
            mSavedComposerNodes.remove(node);
            mSavedComposerNodes.add(node);
        }
        String path = ResourceHelper.getComposePath(mContext) + node.getNode();
        return mRenderManager.updateComposerNodes(path, node.getKey(), node.getValue()) == BEF_RESULT_SUC;
    }

    /**
     * 开启或者关闭贴纸 如果path为空 关闭贴纸
     * 注意 贴纸和Composer类型的特效（美颜、美妆）是互斥的，如果同时设置设置，后者会取消前者的效果
     * Turn on or off the sticker. If path is empty, turn off
     * Note that the stickers and Composer types of special effects (beauty, makeup) are mutually exclusive
     * If you set at the same the, the latter will cancel the effect of the former
     *
     * @param path 贴纸素材的文件路径
     */
    public boolean setSticker(String path) {
        mStickerResource = path;
        return mRenderManager.setSticker(path);
    }

    /**
     * 设置滤镜强度
     * Set the intensity of the filter
     *
     * @param intensity intensity 参数值
     * @return 是否成功  if it is successful
     */
    public boolean updateFilterIntensity(float intensity) {
        boolean result = mRenderManager.updateIntensity(BytedEffectConstants.IntensityType.Filter.getId(), intensity);
        if (result) {
            mFilterIntensity = intensity;
        }
        return result;

    }

    /**
     * 切换摄像头后恢复特效设置
     * Restore beauty, filter and other Settings
     */
    public void recoverStatus() {
        if (!TextUtils.isEmpty(mFilterResource)) {
            setFilter(mFilterResource);

        }
        if (!TextUtils.isEmpty(mStickerResource)) {
            setSticker(mStickerResource);
        }

        if (mComposeNodes.length > 0) {
            setComposeNodes(mComposeNodes);
            for (ComposerNode node : mSavedComposerNodes) {
                updateComposeNode(node, false);
            }
        }
        updateFilterIntensity(mFilterIntensity);
    }

    /**
     * 处理纹理输入 processTexture texture
     * @param srcTextureId input texture id
     * @param textureFormat texture format, TEXTURE_2D or TEXTURE_OES
     * @param width texture width
     * @param height texture height
     * @param rotation texture rotation, refers to{@link BytedEffectConstants.Rotation}
     * @return texture id that is processed or the source texture id if not enabled
     */
    public int processTexture(int srcTextureId, int textureFormat, int width, int height, int rotation, long timestamp) {
        if (!mInitialized || !mIsEffectOn) return srcTextureId;

        TextureFormat format = textureFormat == GLES11Ext.GL_TEXTURE_EXTERNAL_OES
                ? TextureFormat.Texture_Oes : TextureFormat.Texture2D;

        int tempTextureId = mEffectRender.drawFrameOffScreen(srcTextureId, format, width, height);
        int dstTextureId = mEffectRender.prepareTexture(width, height);
        mRenderManager.processTexture(tempTextureId, dstTextureId,
                width, height, toByteEffectRotation(rotation), timestamp);
        return dstTextureId;
    }

    private BytedEffectConstants.Rotation toByteEffectRotation(int rotation) {
        switch (rotation) {
            case 90: return BytedEffectConstants.Rotation.CLOCKWISE_ROTATE_90;
            case 180: return BytedEffectConstants.Rotation.CLOCKWISE_ROTATE_180;
            case 270: return BytedEffectConstants.Rotation.CLOCKWISE_ROTATE_270;
            default: return BytedEffectConstants.Rotation.CLOCKWISE_ROTATE_0;
        }
    }
}
