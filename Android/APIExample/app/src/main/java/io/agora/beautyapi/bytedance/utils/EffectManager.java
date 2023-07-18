/*
 * MIT License
 *
 * Copyright (c) 2023 Agora Community
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

package io.agora.beautyapi.bytedance.utils;

import static com.bytedance.labcv.effectsdk.BytedEffectConstants.BytedResultCode.BEF_RESULT_SUC;

import android.app.ActivityManager;
import android.content.Context;
import android.content.Intent;
import android.content.pm.ConfigurationInfo;
import android.text.TextUtils;

import androidx.localbroadcastmanager.content.LocalBroadcastManager;

import com.bytedance.labcv.effectsdk.BefFaceInfo;
import com.bytedance.labcv.effectsdk.BefHandInfo;
import com.bytedance.labcv.effectsdk.BefSkeletonInfo;
import com.bytedance.labcv.effectsdk.BytedEffectConstants;
import com.bytedance.labcv.effectsdk.RenderManager;

import java.nio.ByteBuffer;
import java.util.Arrays;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Objects;
import java.util.Set;

import io.agora.beautyapi.bytedance.utils.timer_record.LogTimerRecord;

/** {zh} 
 * Created on 5/8/21 10:53 AM
 * 特效接入封装类，确保所有调用在GL线程中使用
 */
/** {en} 
 * Created on 5/8/21 10:53 AM
 *  Effect access encapsulation class, ensure that all calls are used in GL threads
 */

public class EffectManager {
    private final String mlicensePath;
    protected Context mContext;
    protected RenderManager mRenderManager;
    private EffectResourceProvider mResourceProvider;
    private OnEffectListener mOnEffectListener;
    public static final boolean USE_PIPELINE = false;
    private String mFilterResource;
    private String mStickerResource;
    private Set<SavedComposerItem> mSavedComposerNodes = new HashSet<>();
    private Set<String> mExistResourcePath = new HashSet<>();
    private boolean mNeedLoadResource = false;
    private boolean mEnableSyncLoadResource = true;
    private float mFilterIntensity = 0f;


    public EffectManager(Context context, EffectResourceProvider mResourceProvider, String licensePath) {
        mContext = context;
        this.mResourceProvider = mResourceProvider;
        mRenderManager = new RenderManager();
        mlicensePath = licensePath;
    }

    /** {zh} 
     * @brief 初始化特效SDK，确保在gl线程中执行
     * initialize SDK, must run in gl thread
     */
    /** {en} 
     * @brief Initialize the special effects SDK, make sure to execute
     * initialize SDK in gl thread, must run in gl thread
     */

   public int init(){
        LogUtils.d("Effect SDK version =" + mRenderManager.getSDKVersion());



        ActivityManager am = (ActivityManager) mContext.getSystemService(mContext.ACTIVITY_SERVICE);
        ConfigurationInfo ci = am.getDeviceConfigurationInfo();
        int renderapi = (ci.reqGlEsVersion >= 0x30000)?1:0;

        int ret = mRenderManager.init(mContext, mResourceProvider.getModelPath(), mlicensePath, USE_PIPELINE,false,renderapi);
        if (!checkResult("mRenderManager.init", ret)) return ret;
        // AR 启用内置陀螺仪采集
        setUseBuiltinSensor(true);
        set3Buffer(false);
        if (mOnEffectListener != null) {
            mOnEffectListener.onEffectInitialized();
        }
        return ret;

    }


    /** {zh} 
     * @param srcTexture     输入纹理 ID
     *                       input texture ID
     * @param dstTexture 输出纹理ID
     *                   output texture ID
     * @param width  width
     * @param height height
     * @param sensorRotation   phone rotation
     * @param timestamp        timeStamp
     * @return 输出 2D 纹理 output texture
     * @brief 处理纹理
     * process texture
     * @details 此函数只可处理人脸为正的2D格式纹理，确保在gl线程中执行
     */
    /** {en} 
     * @param srcTexture      input texture ID
     *                       input texture ID
     * @param dstTexture  output texture ID
     *                   output texture ID
     * @param width  width
     * @param height height
     * @param sensorRotation   phone rotation
     * @param timestamp        timeStamp
     * @return  output 2D texture output texture
     * @brief  processing texture
     * processing texture
     * @details This function can only process 2D format textures with positive faces, ensuring execution in gl threads
     */

    public boolean process(int srcTexture,int dstTexture, int width, int height, BytedEffectConstants.Rotation sensorRotation, long timestamp){
        LogTimerRecord.RECORD("effectProcess");
        if (mEnableSyncLoadResource && mNeedLoadResource){
            mRenderManager.loadResourceWithTimeout(-1);
            mNeedLoadResource = false;
        }
        //  {zh} 注册Sensor监听，SLAM AR需要  {en} Registered Sensor Listening, SLAM AR Required
//       setUseBuiltinSensor(true);
        boolean ret = mRenderManager.processTexture(srcTexture,dstTexture, width,height, sensorRotation, timestamp);
        LogTimerRecord.STOP("effectProcess");
        return ret;
    }

    /** {zh} 
     * @brief 销毁特效 SDK，确保在 gl 线程中执行
     * destroy SDK，must run in gl thread
     */
    /** {en} 
     * @brief Destroy special effects SDK, make sure to execute
     * destroy SDK in gl thread, must run in gl thread
     */

    public int destroy(){
        LogUtils.d("destroyEffectSDK");
        mRenderManager.release();
        mNeedLoadResource = false;
        mExistResourcePath.clear();
        LogUtils.d("destroyEffectSDK finish");
        return 0;

    }

    /** {zh} 
     * @param listener 监听函数
     *                 listener
     * @brief 设置特效监听回调接口
     * set listener
     */
    /** {en} 
     * @param listener  Listener function
     *                 Listener
     * @brief  Set special effects monitor callback interface
     * set listener
     */

    public void setOnEffectListener(OnEffectListener listener){
        mOnEffectListener = listener;

    }

    /** {zh} 
     * @param path 相对路径
     *             relative path
     * @brief 设置滤镜路径
     * set filter path
     * @details 相对 FilterResource.bundle/Filter 路径，为 null 时关闭滤镜
     * relative path to FilterResource.bundle/Filter, close filter if null
     */
    /** {en} 
     * @param path Relative path
     *             Relative path
     * @brief Set filter path
     * Set filter path
     * @details Relative FilterResource .bundle/Filter path, close filter when null
     * relative path to FilterResource.bundle/Filter, close filter if null
     */

    public boolean setFilter(String path){
        if (!TextUtils.isEmpty(path)) {
            path = mResourceProvider.getFilterPath(path);
        }
        mFilterResource = path;
        return mRenderManager.setFilter(path);

    }

    public boolean setFilterAbs(String absPath) {
        mFilterResource = absPath;
        return mRenderManager.setFilter(absPath);
    }

    /** {zh} 
     * @param intensity 滤镜强度，0-1
     *                  filter intensity, from 0 to 1
     * @brief 设置滤镜强度
     * set filter intensity
     */
    /** {en} 
     * @param intensity Filter intensity, 0-1
     *                  filter intensity, from 0 to 1
     * @brief Set filter intensity
     * Set filter intensity
     */

    public boolean updateFilterIntensity(float intensity){
        boolean result = mRenderManager.updateIntensity(BytedEffectConstants.IntensityType.Filter.getId(), intensity);
        if (result) {
            mFilterIntensity = intensity;
        }
        return result;
    }

    /** {zh} 
     * @param nodes 特效素材相对 ComposeMakeup.bundle/ComposeMakeup 的路径
     *              effect resource relative path to ComposeMakeup.bundle/ComposeMakeup
     * @brief 设置组合特效
     * set composer effects
     * @details 设置 ComposeMakeup.bundle 下的所有功能，包含美颜、美形、美体、美妆等
     * set effects below ComposeMakeup.bundle, includes beauty, reshape, body and makeup ect.
     */
    /** {en} 
     * @param nodes The path of the special effects material relative to ComposeMakeup.bundle/ComposeMakeup
     *              effect resource relative path to ComposeMakeup.bundle/ComposeMakeup
     * @brief Set the combination effects
     * set composer effects
     * @details Set all the functions under ComposeMakeup.bundle, including beauty, beauty, body beauty, beauty makeup, etc.
     * set effects below ComposeMakeup.bundle, includes beauty, reshape, body and makeup ect.
     */

    public boolean setComposeNodes(String[] nodes){
        return setComposeNodes(nodes, null);

    }

    /** {zh} 
     * @param nodes 特效素材相对 ComposeMakeup.bundle/ComposeMakeup 的路径
     *              effect resource relative path to ComposeMakeup.bundle/ComposeMakeup
     * @brief 设置组合特效
     * set composer effects
     * @details 设置 ComposeMakeup.bundle 下的所有功能，包含美颜、美形、美体、美妆等
     * set effects below ComposeMakeup.bundle, includes beauty, reshape, body and makeup ect.
     */
    /** {en} 
     * @param nodes The path of the special effects material relative to ComposeMakeup.bundle/ComposeMakeup
     *              effect resource relative path to ComposeMakeup.bundle/ComposeMakeup
     * @brief Set combination effects
     * set composer effects
     * @details Set all the functions under ComposeMakeup.bundle, including beauty, beauty, body beauty, beauty makeup, etc.
     * set effects below ComposeMakeup.bundle, includes beauty, reshape, body and makeup ect.
     */

    public boolean appendComposeNodes(String[] nodes){
        String prefix = mResourceProvider.getComposePath();
        String[] path = new String[nodes.length];
        for (int i = 0; i < nodes.length; i++) {
            path[i] = prefix + nodes[i];
        }
        if (mEnableSyncLoadResource){
            for (String item: path){
                if (!mExistResourcePath.contains(item)){
                    mNeedLoadResource = true;
                    break;
                }
            }
            mExistResourcePath.addAll(Arrays.asList(path));
        }


        boolean result =  mRenderManager.appendComposerNodes(path) == BEF_RESULT_SUC;
        if (mEnableSyncLoadResource && mNeedLoadResource){

            mRenderManager.loadResourceWithTimeout(-1);
            mNeedLoadResource = false;
        }
        return result;

    }

    /** {zh} 
     * 删除特效节点
     * @param nodes
     * @return
     * @details 删除节点同时，需要维护保存节点的状态
     */
    /** {en} 
     * Delete effect nodes
     * @param nodes
     * @return
     * @details Delete nodes at the same time, you need to maintain the state of the saved nodes
     */
    public boolean removeComposeNodes(String[] nodes){
        String prefix = mResourceProvider.getComposePath();
        String[] path = new String[nodes.length];
        for (int i = 0; i < nodes.length; i++) {
            path[i] = prefix + nodes[i];
            Iterator<SavedComposerItem>iterator = mSavedComposerNodes.iterator();
            while (iterator.hasNext()){
                if (TextUtils.equals(iterator.next().node, nodes[i])){
                    iterator.remove();
                }
            }
        }
        if (mEnableSyncLoadResource){
            mExistResourcePath.removeAll(Arrays.asList(path));
        }

        return mRenderManager.removeComposerNodes(path) == BEF_RESULT_SUC;
    }

    /** {zh} 
     * 重置特效节点
     * @param nodes
     * @param tags
     * @return
     * @details 重置节点同时，需要维护保存节点的状态，删除不在节点数组中的节点
     */
    /** {en} 
     * Reset effect node
     * @param nodes
     * @param tags
     * @return
     * @details Reset node at the same time, you need to maintain the state of the saved node and delete the node that is not in the node array
     */
    public boolean setComposeNodes(String[] nodes, String[] tags){
        Iterator<SavedComposerItem>iterator = mSavedComposerNodes.iterator();
        while (iterator.hasNext()){
            if (!contains(nodes,iterator.next().node)){
                iterator.remove();
            }
        }

        String prefix = mResourceProvider.getComposePath();
        String[] path = new String[nodes.length];
        for (int i = 0; i < nodes.length; i++) {
            path[i] = prefix + nodes[i];

        }
        if (mEnableSyncLoadResource){
            for (String item: path){
                if (!mExistResourcePath.contains(item)){
                    mNeedLoadResource = true;
                    break;
                }
            }
            mExistResourcePath.clear();
            mExistResourcePath.addAll(Arrays.asList(path));
        }

        boolean result =  mRenderManager.setComposerNodesWithTags(path, tags) == BEF_RESULT_SUC;
        if (mEnableSyncLoadResource && mNeedLoadResource){

            mRenderManager.loadResourceWithTimeout(-1);
            mNeedLoadResource = false;
        }
        return result;
    }

    /** {zh} 
     * 判断数组中是否包含某个元素
     * @param nodes
     * @param node
     * @return
     */
    /** {en} 
     * Determine whether the array contains an element
     * @param nodes
     * @param node
     * @return
     */
    private boolean contains(String[] nodes, String node){
        boolean contain = false;
        for (String item:nodes){
            if (TextUtils.equals(item,node)){
                contain = true;
            }
        }
        return contain;
    }




    /** {zh} 
     * 更新特效节点的强度
     * @param node 特效节点的名称
     * @param key key名称
     * @param intensity 强度值
     * @return 成功返回true 失败返回false
     */
    /** {en} 
     * Update the strength of the effect node
     * @param node The name of the effect node
     * @param key key name
     * @param intensity The strength value
     * @return Successful return true Failure return false
     */
    public boolean updateComposerNodeIntensity(String node, String key, float intensity){
        SavedComposerItem item = new SavedComposerItem(node, key, intensity);
        if (mSavedComposerNodes.contains(item)){
            mSavedComposerNodes.remove(item);
        }
        mSavedComposerNodes.add(item);
        String path = mResourceProvider.getComposePath() + node;
        LogUtils.d("updateComposerNodes node ="+path+" key = "+ key + " intensity ="+intensity);

        return mRenderManager.updateComposerNodes(path, key, intensity) == BEF_RESULT_SUC;
    }

    /** {zh} 
     * @param path 贴纸路径
     *             relative path of sticker
     * @brief 设置贴纸路径
     * set sticker path
     * @details 贴纸素材的文件路径，相对 StickerResource.bundle 路径，为 null 时为关闭贴纸
     * relative path of sticker resource to StickerResource.bundle, close sticker if null
     */
    /** {en} 
     * @param path Sticker path
     *             Relative path of sticker
     * @brief Set sticker path
     * Sticker path
     * @details Sticker material file path, relative StickerResource .bundle path, closed sticker when null
     * relative path of sticker resource to StickerResource.bundle, close sticker if null
     */

    public boolean setSticker(String path){
        if (!TextUtils.isEmpty(path)) {
            path = mResourceProvider.getStickerPath(path);
        }
        mStickerResource = path;
        return mRenderManager.setSticker(path);
    }

    /** {zh} 
     * @brief 设置贴纸的绝对路径
     * @details 贴纸素材的文件路径，在 SD 卡上的绝对路径，为 null 是为关闭贴纸
     * @param absPath 贴纸路径
     *                sticker path
     */
    /** {en} 
     * @brief Set the absolute path of the sticker
     * @details the file path of the sticker material, the absolute path on the SD card, null is to close the sticker
     * @param absPath  sticker path
     *                sticker path
     */

    public boolean setStickerAbs(String absPath){
        mStickerResource = absPath;
        return mRenderManager.setSticker(absPath);

    }

    /** {zh} 
     * @param features 功能数组，外部分配大小
     *                 features array, allocate in outer
     * @brief 获取 SDK 支持的功能，一般为测试用
     * get available features in SDK, just for test
     */
    /** {en} 
     * @param features  function array, external allocation size
     *                 features array, allocated in outer
     * @brief  get SDK support functions, generally for testing
     * get available features in SDK, just for test
     */

    public boolean getAvailableFeatures(String[] features){
        return mRenderManager.getAvailableFeatures(features);

    }

    /** {zh} 
     * @return 人脸检测结果 face result
     * @brief 获取特效 SDK 中的人脸检测结果
     * get face result in effect SDK
     */
    /** {en} 
     * @return Face detection result face result
     * @brief get special effects face detection result in SDK
     * get face result in effect SDK
     */

    public BefFaceInfo getFaceDetectResult(){
        return mRenderManager.getFaceDetectResult();
    }

    /** {zh} 
     * @return 手势检测结果 hand result
     * @brief 获取特效 SDK 中的手势检测结果
     * get hand result in effect SDK
     */
    /** {en} 
     * @return Gesture detection result hand result
     * @brief get special effects gesture detection result in SDK
     * get hand result in effect SDK
     */

    public BefHandInfo getHandDetectResult(){
        return mRenderManager.getHandDetectResult();

    }

    /** {zh} 
     * @return 人体检测结果 skeleton result
     * @brief 获取特效 SDK 中的人体检测结果
     * get skeleton result in effect SDK
     */
    /** {en} 
     * @return Human body detection result skeleton result
     * @brief get special effects human body detection result in SDK
     * get skeleton result in effect SDK
     */

    public BefSkeletonInfo getSkeletonDetectResult(){
        return mRenderManager.getSkeletonDetectResult();

    }

    /** {zh} 
     * @param stickerType mask 类型，具体查看 BytedEffectConstants.FaceMaskType
     *             stickerType of mask, see more in BytedEffectConstants.FaceMaskType
     * @return 人脸检测结果 face mask result
     * @brief 获取特效 SDK 中的人脸 mask 结果
     * get face mask result in effect SDK
     */
    /** {en} 
     * @param stickerType mask type, specifically view BytedEffectConstants.FaceMaskType
     *             stickerType of mask, see more in BytedEffectConstants.FaceMaskType
     * @return  face detection results face mask results
     * @brief  get special effects face mask results in SDK
     * get face mask results in effect SDK
     */

    public BefFaceInfo getFaceMaskResult(BytedEffectConstants.FaceMaskType stickerType){
        BefFaceInfo faceInfo = new BefFaceInfo();
        mRenderManager.getFaceMaskResult(stickerType, faceInfo);
        return faceInfo;
    }

    /** {zh} 
     * @brief 处理触摸事件
     * @param eventCode 触摸事件类型
     * @param x 触摸位置，0-1
     * @param y 触摸位置，0-1
     * @param force 压力值
     * @param majorRadius 触摸范围
     * @param pointerId 触摸点 id
     * @param pointerCount 触摸点数量
     */
    /** {en} 
     * @brief Handle touch events
     * @param eventCode Touch event type
     * @param x Touch position, 0-1
     * @param y Touch position, 0-1
     * @param force Pressure value
     * @param majorRadius Touch range
     * @param pointerId Touch point id
     * @param pointerCount Touch point number
     */
    public boolean processTouch(BytedEffectConstants.TouchEventCode eventCode, float x, float y, float force, float majorRadius, int pointerId, int pointerCount) {
        return mRenderManager.processTouch(eventCode, x, y, force, majorRadius, pointerId, pointerCount) == BEF_RESULT_SUC;
    }

    /** {zh} 
     * @brief 处理触摸手势事件
     * @param eventCode 手势事件类型
     * @param x 触摸位置，0-1，缩放手势表示缩放比例，旋转手势表示旋转角度
     * @param y 触摸位置，0-1
     * @param dx 移动距离
     * @param dy 移动距离
     * @param factor 缩放因数
     */
    /** {en} 
     * @brief Handle touch gesture events
     * @param eventCode gesture event type
     * @param x touch position, 0-1, zoom gesture indicates zoom scale, rotation gesture indicates rotation angle
     * @param y touch position, 0-1
     * @param dx moving distance
     * @param dy moving distance
     * @param factor zoom factor
     */
    public boolean processGesture(BytedEffectConstants.GestureEventCode eventCode, float x, float y, float dx, float dy, float factor) {
        return mRenderManager.processGesture(eventCode, x, y, dx, dy, factor) == BEF_RESULT_SUC;
    }

    /** {zh} 
     * @param key       纹理名称
     *                  name of texture
     * @param imagePath 图像路径
     *                  path of texture
     * @brief 通过文件设置 render cache texture
     * @details 传入一个固定名字的纹理给到 SDK，传入图片路径，SDK 会将其解析成纹理,不支持解析文件exf角度信息,
     * 对于带旋转角度的图片，请使用传入ByteBuffer的接口
     * Send a certain name texture to SDK. use image path, and SDK will convert it to texture，but
     * not support decode exf rotation param，please use another override method which use ByteBuffer as input
     */
    /** {en} 
     * @param key        texture name
     *                  name of texture
     * @param imagePath image path
     *                  path of texture
     * @brief through file settings render cache texture
     * @details pass a texture with a fixed name to SDK, pass in picture path, SDK will parse it into texture, does not support parsing file exf rotation information,
     *  For pictures with rotation angle, please use the interface passed into ByteBuffer
     * Send a certain name texture to SDK. use image path, and SDK will convert it to texture, but
     * not support decoding exf rotation param, please use another override method which use ByteBuffer as input
     */

    public boolean setRenderCacheTexture(String key, String imagePath){
        return mRenderManager.setRenderCacheTexture(key, imagePath) == BEF_RESULT_SUC;

    }

    /** {zh} 
     * @param key      key 纹理名称
     *                 name of texture
     * @param buffer   ByteBuffer
     * @param width    width
     * @param height   height
     * @param stride   stride
     * @param format   format，仅支持 RGBA
     *                 only RGBA supported
     * @param rotation rotation
     * @brief 通过 buffer 设置 render cache texture
     * @details 传入一个固定名字的纹理给到 SDK，传入 BEBuffer，SDK 会将其解析成纹理
     */
    /** {en} 
     * @param key      key texture name
     *                 name of texture
     * @param buffer   ByteBuffer
     * @param width    width
     * @param height   height
     * @param height   stride
     * @param stride   format
     *                 format, only supports RGBA
     * @param format RGBA supported rotation
     * @brief rotation Set rendering cache texture
     * @details through buffer Incoming a texture with a fixed name to
     */

    public boolean setRenderCacheTexture(String key, ByteBuffer buffer, int width, int height, int stride, BytedEffectConstants.PixlFormat format, BytedEffectConstants.Rotation rotation){
        return mRenderManager.setRenderCacheTextureWithBuffer(key, buffer, width, height, stride, format, rotation) == BEF_RESULT_SUC;

    }



    /** {zh} 
     * @param usePipeline 是否开启并行渲染
     *                    whether use pipeline
     * @brief 是否开启并行渲染
     * whether use pipeline
     * @details 特效 SDK 内部工作分为两部分，算法检测和特效渲染，当开启并行渲染之后，
     * 算法检测和特效渲染将在不同线程执行，以充分利用多线程进行加速，
     * 但会导致渲染效果延迟一帧
     * there are two parts of effect SDK, algorithm detector and effect render,
     * when usePipeline is true, they will work in different thread to accelerate,
     * with one frame delay
     */
    /** {en} 
     * @param usePipeline  whether to turn on parallel rendering
     *                    whether to use pipeline
     * @brief whether to turn on parallel rendering
     * whether to use pipeline
     * @details special effects SDK internal work is divided into two parts, algorithm detection and special effects rendering. After turning on parallel rendering,
     * algorithm detection and special effects rendering will be executed in different threads to make full use of multi-threads for acceleration,
     * but it will cause the rendering effect to be delayed by one frame
     * there are two parts of effect SDK, algorithm detector and effect render,
     * when usePipeline is true, they will work in different thread to accelerate,
     * with one frame delay
     */

    public boolean setPipeline(boolean usePipeline){
        return mRenderManager.setPipeline(usePipeline);

    }

    /** {zh} 
     * @param use3Buffer 是否开启 3buffer
     *                   whether use 3buffer
     * @brief 是否开启 3-buffer
     * whether use 3-buffer
     * @details 当开启并行渲染之后，由于算法和特效在不同线程执行，所以需要一些线程同步的工作。
     * 当不开启 3buffer 的时候，SDK 会将传进来的每一帧进行拷贝，
     * 当开启 3buffer 的时候，SDK 不会拷贝每一帧纹理，要求外部传进来的纹理是一个循环的队列，
     * 即连续的 3 帧纹理 ID 不能相同
     * when we use pipeline, we should do something for thread safe,
     * if we use 3buffer, SDK will copy every input frame,
     * otherwise, SDK will not copy it, but we should confirm every 3 continuous
     * texture ID not same
     */
    /** {en} 
     * @param use3Buffer  Whether to turn on 3buffer
     *                   Whether to use 3buffer
     * @brief  Whether to turn on 3-buffer
     * Whether to use 3-buffer
     * @details When parallel rendering is turned on, since the algorithm and special effects are executed in different threads, some thread synchronization work is required.
     *  When 3buffer is not turned on, the SDK will copy every frame passed in,
     *  When 3buffer is turned on, the SDK will not copy every frame texture, requiring the external incoming texture to be a circular queue,
     *  That is, 3 consecutive frames of texture ID cannot be the same
     * when we use pipeline, we should do something for thread safe,
     * if we use 3buffer, SDK will copy every input frame,
     * otherwise, SDK will not copy it, but we should confirm every 3 continuous
     * texture ID is not the same
     */

    public boolean set3Buffer(boolean use3Buffer){
        return mRenderManager.set3Buffer(use3Buffer);

    }

    public void setSyncLoadResource(boolean flag){
        mEnableSyncLoadResource = flag;
    }

    /** {zh} 
     * @brief 相机切换回调
     * called when camera changed
     */
    /** {en} 
     * @brief Camera toggle callback
     * called when camera changed
     */

    public void onCameraChanged(){
        mRenderManager.cleanPipeline();
    }

    /** {zh} 
     * @param isFront 是否为前置摄像头
     *                whether it is front camera
     * @brief 设置相机位置
     * set camera position
     */
    /** {en} 
     * @param isFront whether it is front camera
     *                whether it is front camera
     * @brief set camera position
     * set camera position
     */

    public void setCameraPosition(boolean isFront){
        if (null == mRenderManager) return;
        mRenderManager.setCameraPostion(isFront);
    }



    /** {zh} 
     * @brief 恢复状态
     * recover state
     * @details 当调用 destroy 和 init 之后，SDK 实例会重新创建，之前设置的特效会消失，
     * 此时可用此函数，将之前设置的特效恢复
     * after called destroy and init, SDK instance will recreate, and the effects we set
     * will be removed, we can use it to recover effect
     */
    /** {en} 
     * @brief Recovery state
     * recover state
     * @details When destroy and init are called, the SDK instance will be recreated, and the previously set effects will disappear,
     * This function can be used at this time to restore the previously set effects
     * after called destroy and init, SDK instance will recreate, and the effects we set
     * will be removed, we can use it to recover effect
     */

    public void recoverStatus(){
        LogUtils.d("recover status");
        if (!TextUtils.isEmpty(mFilterResource)) {
            mRenderManager.setFilter(mFilterResource);
        }
        if (!TextUtils.isEmpty(mStickerResource)) {
            mRenderManager.setSticker(mStickerResource);
        }
        LogUtils.d("mSavedComposerNodes size ="+mSavedComposerNodes.size()+"  "+mSavedComposerNodes);

        if (mSavedComposerNodes.size() > 0) {
            String[] nodes = getSavedComposerNodes(mSavedComposerNodes);
            String prefix = mResourceProvider.getComposePath();
            String[] path = new String[nodes.length];
            for (int i = 0; i < nodes.length; i++) {
                path[i] = prefix + nodes[i];
            }
            if (mEnableSyncLoadResource){
                for (String item: path){
                    if (!mExistResourcePath.contains(item)){
                        mNeedLoadResource = true;
                        break;
                    }
                }
                mExistResourcePath.clear();
                mExistResourcePath.addAll(Arrays.asList(path));
            }
             mRenderManager.setComposerNodes(path);
            if (mEnableSyncLoadResource && mNeedLoadResource){
                mRenderManager.loadResourceWithTimeout(-1);
                mNeedLoadResource = false;
            }

            for (SavedComposerItem item : mSavedComposerNodes) {
                String nodePath = prefix + item.node;
                LogUtils.d("updateComposerNodes node ="+nodePath+" key = "+ item.key + " intensity ="+item.intensity);

                mRenderManager.updateComposerNodes(nodePath, item.key, item.intensity);
            }
        }
        updateFilterIntensity(mFilterIntensity);
    }

    private String[] getSavedComposerNodes(Set<SavedComposerItem> composerItems){
        if (composerItems == null || composerItems.size() == 0){
            return new String[0];
        }
        Set<String> mComposeNodes = new HashSet<>();
        for (SavedComposerItem item : composerItems) {
            mComposeNodes.add(item.node);
        }
        return mComposeNodes.toArray(new String[mComposeNodes.size()]);
    }

    public interface OnEffectListener {
        void onEffectInitialized();
    }

    private static class SavedComposerItem {
        String node;
        String key;
        float intensity;

        public SavedComposerItem(String node, String key, float value) {
            this.node = node;
            this.key = key;
            this.intensity = value;
        }

        @Override
        public boolean equals(Object o) {
            if (this == o) return true;
            if (o == null || getClass() != o.getClass()) return false;
            SavedComposerItem that = (SavedComposerItem) o;
            return Objects.equals(node, that.node) &&
                    Objects.equals(key, that.key);
        }

        @Override
        public int hashCode() {
            return Objects.hash(node, key);
        }
    }

    protected boolean checkResult(String msg, int ret) {
        if (ret != 0 && ret != -11 && ret != 1) {
            String log = msg + " error: " + ret;
            LogUtils.e(log);
            String toast = RenderManager.formatErrorCode(ret);
            if (toast == null) {
                toast = log;
            }
            Intent intent = new Intent(Config.CHECK_RESULT_BROADCAST_ACTION);
            intent.putExtra("msg", toast);
            LocalBroadcastManager.getInstance(mContext).sendBroadcast(intent);
            return false;
        }
        return true;
    }


    public boolean deviceConfig(boolean accelerator, boolean gyroscope, boolean gravity, boolean orientation){
        return mRenderManager.deviceConfig(accelerator, gyroscope, gravity,orientation) == BEF_RESULT_SUC;
    }

    public  boolean onOrientationChanged(double[] value, int num, double timeStamp){
        return  mRenderManager.onOrientationChanged(value, num, timeStamp) == BEF_RESULT_SUC;

    }

    public boolean onAcceleratorChanged(double x, double y, double z, double timeStamp){
        return mRenderManager.onAcceleratorChanged(x,y,z,timeStamp) == BEF_RESULT_SUC;
    }

    public boolean onGravityChanged(double x, double y, double z, double timeStamp){
        return mRenderManager.onGravityChanged(x,y,z,timeStamp) == BEF_RESULT_SUC;
    }

    public boolean onGyroscopeChanged(double x, double y, double z, double timeStamp){
        return mRenderManager.onGyroscopeChanged(x, y,z,timeStamp) == BEF_RESULT_SUC;
    }

    public boolean setDeviceRotation(float[] quaternion){
        return mRenderManager.setDeviceRotation(quaternion) == BEF_RESULT_SUC;
    }

    public boolean setUseBuiltinSensor(boolean flag){
        LogUtils.d("setUseBuiltinSensor "+flag);
        return mRenderManager.useBuiltinSensor(flag) == BEF_RESULT_SUC;
    }

}
