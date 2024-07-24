//
// Created on 2024/7/24.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef APIEXAMPLE_ORIGIN_VIDEO_DATA_H
#define APIEXAMPLE_ORIGIN_VIDEO_DATA_H

#include "AgoraMediaBase.h"
#include "audio_render.h"
#include "utils.h"
#include "napi/native_api.h"
#include "IAgoraRtcEngine.h"

static thread_local napi_ref g_origin_video_data_ref = nullptr;

class OriginVideoData : public agora::media::IVideoFrameObserver {
public:
    OriginVideoData(uintptr_t rtcEngineHandler);
    static napi_value Init(napi_env env, napi_value exports);
    static void Destructor(napi_env env, void *nativeObject, void *finalize_hint);

private:
    explicit OriginVideoData();
    ~OriginVideoData();

    void SaveI420Buffer(const uint8_t *buf, int width, int height, std::string filename);

    static napi_value New(napi_env env, napi_callback_info info);
    static napi_value Enable(napi_env env, napi_callback_info info);
    static napi_value TakeSnapshot(napi_env env, napi_callback_info info);

    // IVideoFrameObserver
    bool onCaptureVideoFrame(agora::rtc::VIDEO_SOURCE_TYPE sourceType, VideoFrame &videoFrame);
    bool onPreEncodeVideoFrame(agora::rtc::VIDEO_SOURCE_TYPE sourceType, VideoFrame &videoFrame) override {
        return false;
    }
    bool onMediaPlayerVideoFrame(VideoFrame &videoFrame, int mediaPlayerId) override { return false; }
    bool onRenderVideoFrame(const char *channelId, agora::rtc::uid_t remoteUid, VideoFrame &videoFrame) override {
        return false;
    }
    bool onTranscodedVideoFrame(VideoFrame &videoFrame) override { return false; }
    VIDEO_FRAME_PROCESS_MODE getVideoFrameProcessMode() override;
    agora::media::base::VIDEO_PIXEL_FORMAT getVideoFormatPreference() override;
    bool getRotationApplied() override { return false; }
    bool getMirrorApplied() override { return false; }
    uint32_t getObservedFramePosition() override;

    agora::rtc::IRtcEngine *rtcEngine_;
    napi_env env_;
    napi_ref wrapper_;
    bool takeSnapshot_;
};

#endif // APIEXAMPLE_ORIGIN_VIDEO_DATA_H
