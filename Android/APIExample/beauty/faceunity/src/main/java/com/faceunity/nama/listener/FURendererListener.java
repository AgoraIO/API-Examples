package com.faceunity.nama.listener;

import com.faceunity.core.enumeration.FUAIProcessorEnum;

/**
 * DESC：FURenderer状态回调监听
 * Created on 2021/4/29
 */
public interface FURendererListener {


    /**
     * 识别到的人脸或人体数量发生变化
     *
     * @param type   类型
     * @param status 数量
     */
    void onTrackStatusChanged(FUAIProcessorEnum type, int status);


    /**
     * 统计每 10 帧的平均数据，FPS 和渲染函数调用时间
     *
     * @param fps      FPS
     * @param callTime 渲染函数调用时间
     */
    void onFpsChanged(double fps, double callTime);


}
