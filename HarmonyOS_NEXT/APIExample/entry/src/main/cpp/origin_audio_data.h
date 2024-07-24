//
// Created on 2024/7/24.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef APIEXAMPLE_ORIGIN_AUDIO_DATA_H
#define APIEXAMPLE_ORIGIN_AUDIO_DATA_H

#include "AgoraMediaBase.h"
#include "audio_render.h"
#include "utils.h"
#include "napi/native_api.h"
#include "IAgoraRtcEngine.h"

static thread_local napi_ref g_origin_audio_data_ref = nullptr;

class OriginAudioData : public agora::media::IAudioFrameObserver {
public:
    OriginAudioData(uintptr_t rtcEngineHandler);
    static napi_value Init(napi_env env, napi_value exports);
    static void Destructor(napi_env env, void *nativeObject, void *finalize_hint);

private:
    explicit OriginAudioData();
    ~OriginAudioData();

    static napi_value New(napi_env env, napi_callback_info info);
    static napi_value Enable(napi_env env, napi_callback_info info);

    // IAudioFrameObserver
    bool onEarMonitoringAudioFrame(AudioFrame &audioFrame) override { return false; }
    bool onPlaybackAudioFrameBeforeMixing(const char *channelId, agora::rtc::uid_t uid,
                                          AudioFrame &audioFrame) override {
        return false;
    }
    bool onMixedAudioFrame(const char *channelId, AudioFrame &audioFrame) override { return false; }
    bool onPlaybackAudioFrameBeforeMixing(const char *channelId, const char *userId, AudioFrame &audioFrame) override {
        return false;
    }
    bool onPlaybackAudioFrame(const char *channelId, AudioFrame &audioFrame) override { return false; }
    bool onRecordAudioFrame(const char *channelId, AudioFrame &audioFrame) override;
    int getObservedAudioFramePosition() override;
    AudioParams getMixedAudioParams() override { return AudioParams(); }
    AudioParams getEarMonitoringAudioParams() override { return AudioParams(); }
    AudioParams getPlaybackAudioParams() override { return AudioParams(); }
    AudioParams getRecordAudioParams() override;

    agora::rtc::IRtcEngine *rtcEngine_;
    napi_env env_;
    napi_ref wrapper_;
};

#endif // APIEXAMPLE_ORIGIN_AUDIO_DATA_H
