//
// Created on 2024/7/24.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef APIEXAMPLE_CUSTOM_AUDIO_RENDER_H
#define APIEXAMPLE_CUSTOM_AUDIO_RENDER_H

#include "AgoraMediaBase.h"
#include "audio_render.h"
#include "utils.h"
#include "napi/native_api.h"
#include "IAgoraRtcEngine.h"
#include "audio_render.h"

static thread_local napi_ref g_custom_audio_render_ref = nullptr;

class CustomAudioRender : public agora::media::IAudioFrameObserver {
public:
    CustomAudioRender(uintptr_t rtcEngineHandler);
    static napi_value Init(napi_env env, napi_value exports);
    static void Destructor(napi_env env, void *nativeObject, void *finalize_hint);

private:
    explicit CustomAudioRender();
    ~CustomAudioRender();

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
    bool onPlaybackAudioFrame(const char *channelId, AudioFrame &audioFrame) override;
    bool onRecordAudioFrame(const char *channelId, AudioFrame &audioFrame) override { return false; }
    int getObservedAudioFramePosition() override;
    AudioParams getMixedAudioParams() override { return AudioParams(); }
    AudioParams getEarMonitoringAudioParams() override { return AudioParams(); }
    AudioParams getPlaybackAudioParams() override;
    AudioParams getRecordAudioParams() override { return AudioParams(); }

    agora::rtc::IRtcEngine *rtcEngine_;
    napi_env env_;
    napi_ref wrapper_;
    
    AudioRenderOHOS* audioRender_ = nullptr;
};

#endif // APIEXAMPLE_CUSTOM_AUDIO_RENDER_H
