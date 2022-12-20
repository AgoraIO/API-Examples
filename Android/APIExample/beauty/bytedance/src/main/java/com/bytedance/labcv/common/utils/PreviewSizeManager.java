package com.bytedance.labcv.common.utils;

import android.graphics.Rect;

/** {zh} 
 * Created on 2/7/21 8:00 PM
 * 主要用于在 view（手机屏幕） 和 preview（预览图像） 之间做坐标系转换，
 * view 和 preview 之间由于比例、尺寸不一样，需要对 preview 做缩放、剪裁，
 * 造成view 和 preview 之间的坐标需要做一层转换才能对得上
 */
/** {en} 
 * Created on 2/7/21 8:00 PM
 *  is mainly used to do coordinate system conversion between view (mobile phone screen) and preview (preview image). Due to the different proportions and sizes between
 * view and preview, the preview needs to be scaled and clipped.
 *  The coordinates between view and preview need to be converted to match
 */

public class PreviewSizeManager {
    private int mPreviewWidth;
    private int mPreviewHeight;
    private int mViewWidth;
    private int mViewHeight;
    private boolean mFitCenter;

    private float mWidthRatio;
    private float mHeightRatio;

    public static PreviewSizeManager getInstance() {
        return InstanceHolder.sInstance;
    }

    /** {zh} 
     * 更新 view 和 preview 的尺寸和剪裁方式
     * @param viewWidth view 宽
     * @param viewHeight view 高
     * @param previewWidth preview 宽
     * @param previewHeight preview 高
     * @param fitCenter 是否为居中 fit
     */
    /** {en} 
     * Update the size and tailoring of view and preview
     * @param viewWidth view width
     * @param viewHeight view height
     * @param previewWidth preview width
     * @param previewHeight preview height
     * @param fitCenter  whether it is a center fit
     */

    public void update(int viewWidth, int viewHeight, int previewWidth, int previewHeight, boolean fitCenter) {
        mPreviewWidth = previewWidth;
        mPreviewHeight = previewHeight;
        mViewWidth = viewWidth;
        mViewHeight = viewHeight;
        mFitCenter = fitCenter;

        mWidthRatio = viewWidth * 1.f / previewWidth;
        mHeightRatio = viewHeight * 1.f / previewHeight;
    }

    /** {zh} 
     * 获取缩放倍数
     * @return 缩放倍数 view -> preview
     */
    /** {en} 
     * Get the zoom multiple
     * @return zoom multiple view - > preview
     */

    public float getRatio() {
        return mFitCenter ? Math.min(mWidthRatio, mHeightRatio) : Math.max(mWidthRatio, mHeightRatio);
    }

    /**
     * Get x in view coordinator according to x in preview coordinator
     * @param x x in preview coordinator
     * @return x in view coordinator
     */
    public float previewToViewX(float x) {
        float ratio = getRatio();
        float offset = mPreviewWidth * (mWidthRatio - ratio) / 2;
        return offset + x * ratio;
    }

    /**
     * Get y in view coordinator according to y in preview coordinator
     * @param y y in preview coordinator
     * @return y in view coordinator
     */
    public float previewToViewY(float y) {
        float ratio = getRatio();
        float offset = mPreviewHeight * (mHeightRatio - ratio) / 2;
        return offset + y * ratio;
    }

    /**
     * Get Rect in view coordinator according to Rect in preview coordinator
     * @param rect rect in preview coordinator
     */
    public void previewToView(Rect rect) {
        rect.set((int)previewToViewX(rect.left), (int)previewToViewY(rect.top),
                (int)previewToViewX(rect.right), (int)previewToViewY(rect.bottom));
    }

    /**
     * Get x in preview coordinator according to x in view coordinator
     * @param x x in view coordinator
     * @return x in preview coordinator
     */
    public float viewToPreviewX(float x) {
        float ratio = getRatio();
        float offset = mPreviewWidth * (mWidthRatio - ratio) / 2;
        return (x - offset) / ratio;
    }

    /**
     * Get y in preview coordinator according to y in view coordinator
     * @param y y in view coordinator
     * @return y in preview coordinator
     */
    public float viewToPreviewY(float y) {
        float ratio = getRatio();
        float offset = mPreviewHeight * (mHeightRatio - ratio) / 2;
        return (y - offset) / ratio;
    }

    /**
     * Get x proportion in preview coordinator according to x in view coordinator
     * @param x x in view coordinator
     * @return x proportion in preview coordinator, 0-1
     */
    public float viewToPreviewXFactor(float x) {
        return viewToPreviewX(x) / mPreviewWidth;
    }

    /**
     * Get y proportion in preview coordinator according to y in view coordinator
     * @param y y in view coordinator
     * @return y proportion in preview coordinator, 0-1
     */
    public float viewToPreviewYFactor(float y) {
        return viewToPreviewY(y) / mPreviewHeight;
    }

    public float viewXFactor(float x) {
        return x / mViewWidth;
    }

    public float viewYFactor(float y) {
        return y / mViewHeight;
    }

    private static class InstanceHolder {
        private static final PreviewSizeManager sInstance = new PreviewSizeManager();
    }
}
