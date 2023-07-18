package io.agora.beautyapi.faceunity.utils.nama;


import android.content.Context;

import com.faceunity.core.enumeration.CameraFacingEnum;
import com.faceunity.core.enumeration.FUAIProcessorEnum;
import com.faceunity.core.enumeration.FUExternalInputEnum;
import com.faceunity.core.enumeration.FUInputBufferEnum;
import com.faceunity.core.enumeration.FUInputTextureEnum;
import com.faceunity.core.enumeration.FUTransformMatrixEnum;

import io.agora.beautyapi.faceunity.utils.nama.listener.FURendererListener;

/**
 * DESC：
 * Created on 2021/4/26
 */
abstract class IFURenderer {

    /**
     * 渲染属性
     */
    protected FUExternalInputEnum externalInputType = FUExternalInputEnum.EXTERNAL_INPUT_TYPE_CAMERA;//数据源类型
    protected FUInputTextureEnum inputTextureType = FUInputTextureEnum.FU_ADM_FLAG_EXTERNAL_OES_TEXTURE;//纹理类型
    protected FUInputBufferEnum inputBufferType = FUInputBufferEnum.FU_FORMAT_NV21_BUFFER;//数据类型
    protected int inputOrientation = 0;//数据源朝向
    protected int deviceOrientation = 90;//手机设备朝向
    protected CameraFacingEnum cameraFacing = CameraFacingEnum.CAMERA_FRONT;  //数据源为相机时候->前后置相机
    protected FUTransformMatrixEnum inputTextureMatrix = FUTransformMatrixEnum.CCROT0_FLIPVERTICAL;//纹理旋转类型
    protected FUTransformMatrixEnum inputBufferMatrix = FUTransformMatrixEnum.CCROT0_FLIPVERTICAL;//图象旋转类型
    protected FUTransformMatrixEnum outputMatrix = FUTransformMatrixEnum.CCROT0;//图象旋转类型

    /**
     * 初始化鉴权
     */
    public abstract void setup(Context context, byte[] auth);


    /**
     * 双输入接口，输入 buffer 和 texture，必须在具有 GL 环境的线程调用
     * 由于省去数据拷贝，性能相对最优，优先推荐使用。
     * 缺点是无法保证 buffer 和纹理对齐，可能出现点位和效果对不上的情况。
     *
     * @param img    NV21 buffer
     * @param texId  纹理 ID
     * @param width  宽
     * @param height 高
     * @return
     */
    public abstract int onDrawFrameDualInput(byte[] img, int texId, int width, int height);

    public abstract int onDrawFrameInput(int texId, int width, int height);
    public abstract int onDrawFrameInput(byte[] img, int width, int height);

    /**
     * 设置检测类型
     *
     * @param type
     */
    public abstract void setAIProcessTrackType(FUAIProcessorEnum type);

    /**
     * 资源释放
     */
    public abstract void release();

    /**
     * 开启合成状态
     */
    public abstract void bindListener(FURendererListener mFURendererListener);


    /**
     * 设置FPS检测
     *
     * @param enable
     */
    public abstract void setMarkFPSEnable(boolean enable);


    /**
     * 获取输入源类型
     *
     * @return
     */
    public FUExternalInputEnum getExternalInputType() {
        return externalInputType;
    }

    /**
     * 设置输入源类型
     *
     * @param externalInputType
     */
    public void setExternalInputType(FUExternalInputEnum externalInputType) {
        this.externalInputType = externalInputType;
    }

    /**
     * 获取输入纹理类型
     *
     * @return
     */
    public FUInputTextureEnum getInputTextureType() {
        return inputTextureType;
    }

    /**
     * 设置输入纹理类型
     *
     * @param inputTextureType
     */
    public void setInputTextureType(FUInputTextureEnum inputTextureType) {
        this.inputTextureType = inputTextureType;
    }

    /**
     * 获取输入Buffer类型
     *
     * @return
     */
    public FUInputBufferEnum getInputBufferType() {
        return inputBufferType;
    }

    /**
     * 设置输入Buffer类型
     *
     * @param inputBufferType
     */
    public void setInputBufferType(FUInputBufferEnum inputBufferType) {
        this.inputBufferType = inputBufferType;
    }

    /**
     * 获取输入数据朝向
     *
     * @return
     */
    public int getInputOrientation() {
        return inputOrientation;
    }

    /**
     * 设置输入数据朝向
     *
     * @param inputOrientation
     */
    public void setInputOrientation(int inputOrientation) {
        this.inputOrientation = inputOrientation;
    }

    /**
     * 获取设备类型
     *
     * @return
     */
    public int getDeviceOrientation() {
        return deviceOrientation;
    }

    /**
     * 设置设备类型
     *
     * @param deviceOrientation
     */
    public void setDeviceOrientation(int deviceOrientation) {
        this.deviceOrientation = deviceOrientation;
    }

    /**
     * 获取设备朝向
     *
     * @return
     */
    public CameraFacingEnum getCameraFacing() {
        return cameraFacing;
    }

    /**
     * 设置设备朝向
     *
     * @param cameraFacing
     */
    public void setCameraFacing(CameraFacingEnum cameraFacing) {
        this.cameraFacing = cameraFacing;
    }

    /**
     * 获取输入纹理旋转类型
     *
     * @return
     */
    public FUTransformMatrixEnum getInputTextureMatrix() {
        return inputTextureMatrix;
    }

    /**
     * 设置输入纹理旋转类型
     *
     * @param inputTextureMatrix
     */
    public void setInputTextureMatrix(FUTransformMatrixEnum inputTextureMatrix) {
        this.inputTextureMatrix = inputTextureMatrix;
    }

    /**
     * 获取输入数据旋转类型
     *
     * @return
     */
    public FUTransformMatrixEnum getInputBufferMatrix() {
        return inputBufferMatrix;
    }

    /**
     * 设置输入数据旋转类型
     *
     * @param inputBufferMatrix
     */
    public void setInputBufferMatrix(FUTransformMatrixEnum inputBufferMatrix) {
        this.inputBufferMatrix = inputBufferMatrix;
    }

    /**
     * 获取输出数据旋转类型
     *
     * @return
     */
    public FUTransformMatrixEnum getOutputMatrix() {
        return outputMatrix;
    }

    /**
     * 设置输出数据旋转类型
     *
     * @param outputMatrix
     */
    public void setOutputMatrix(FUTransformMatrixEnum outputMatrix) {
        this.outputMatrix = outputMatrix;
    }
}
