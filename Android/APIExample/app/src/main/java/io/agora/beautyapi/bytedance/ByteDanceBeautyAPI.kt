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

package io.agora.beautyapi.bytedance

import android.view.View
import io.agora.base.VideoFrame
import io.agora.beautyapi.bytedance.utils.EffectManager
import io.agora.rtc2.Constants
import io.agora.rtc2.RtcEngine

enum class CaptureMode{
    Agora, // 使用声网内部的祼数据接口进行处理
    Custom // 自定义模式，需要自己调用onFrame接口将原始视频帧传给BeautyAPI做处理
}

data class EventCallback(
    /**
     * 统计数据回调，每处理完一帧后会回调一次
     *
     * @param stats 美颜统计数据
     */
    val onBeautyStats: ((stats: BeautyStats)->Unit)? = null,

    /**
     * effectManager在GL线程里初始化完成后回调
     */
    val onEffectInitialized: (()->Unit)? = null,

    /**
     * effectManager在GL线程里销毁完成后回调
     */
    val onEffectDestroyed: (()->Unit)? = null
)

data class BeautyStats(
    val minCostMs:Long, // 统计区间内的最小值
    val maxCostMs: Long, // 统计区间内的最大值
    val averageCostMs: Long // 统计区间内的平均值
)

data class Config(
    val rtcEngine: RtcEngine, // 声网Rtc引擎
    val effectManager: EffectManager, // 美颜SDK处理句柄
    val eventCallback: EventCallback? = null, // 事件回调
    val captureMode: CaptureMode = CaptureMode.Agora, // 处理模式
    val statsDuration: Long = 1000, // 统计区间
    val statsEnable: Boolean = false // 是否开启统计
)

enum class ErrorCode(val value: Int) {
    ERROR_OK(0), // 一切正常
    ERROR_HAS_NOT_INITIALIZED(101), // 没有调用Initialize或调用失败情况下调用了其他API
    ERROR_HAS_INITIALIZED(102), // 已经Initialize成功后再次调用报错
    ERROR_HAS_RELEASED(103), // 已经调用release销毁后还调用其他API
    ERROR_PROCESS_NOT_CUSTOM(104), // 非Custom处理模式下调用onFrame接口从外部传入视频帧
    ERROR_PROCESS_DISABLE(105), // 当调用enable(false)禁用美颜后调用onFrame接口返回
    ERROR_VIEW_TYPE_ERROR(106), // 当调用setupLocalVideo时view类型错误时返回
    ERROR_FRAME_SKIPPED(107), // 当处理帧忽略时在onFrame返回
}

enum class BeautyPreset {
    CUSTOM, // 不使用推荐的美颜参数
    DEFAULT // 默认的
}

fun createByteDanceBeautyAPI(): ByteDanceBeautyAPI = ByteDanceBeautyAPIImpl()

interface ByteDanceBeautyAPI {

    /**
     * 初始化API
     *
     * @param config 配置参数
     * @return 见ErrorCode
     */
    fun initialize(config: Config): Int

    /**
     * 开启/关闭美颜
     *
     * @param enable true：开启; false: 关闭
     * @return 见ErrorCode
     */
    fun enable(enable: Boolean): Int

    /**
     * 本地视图渲染，由内部来处理镜像问题
     *
     * @param view SurfaceView或TextureView
     * @param renderMode 渲染缩放模式
     * @return 见ErrorCode
     */
    fun setupLocalVideo(view: View, renderMode: Int = Constants.RENDER_MODE_HIDDEN): Int

    /**
     * 当ProcessMode==Custom时由外部传入原始视频帧
     *
     * @param videoFrame 原始视频帧
     * @return 见ErrorCode
     */
    fun onFrame(videoFrame: VideoFrame): Int

    /**
     * 声网提供的美颜最佳默认参数
     *
     * @return 见ErrorCode
     */
    fun setBeautyPreset(preset: BeautyPreset = BeautyPreset.DEFAULT): Int

    /**
     * 私参配置，用于不对外api的调用，多用于测试
     */
    fun setParameters(key: String, value: String)

    /**
     * 释放资源，一旦释放后这个实例将无法使用
     *
     * @return 见ErrorCode
     */
    fun release(): Int

}