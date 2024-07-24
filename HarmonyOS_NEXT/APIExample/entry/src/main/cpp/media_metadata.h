//
// Created on 2024/7/24.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef APIEXAMPLE_MEDIA_METADATA_H
#define APIEXAMPLE_MEDIA_METADATA_H
#include "AgoraMediaBase.h"
#include "audio_render.h"
#include "utils.h"
#include "napi/native_api.h"
#include "IAgoraRtcEngine.h"

static thread_local napi_ref g_media_metadata_ref = nullptr;

class MediaMetadata : public agora::rtc::IMetadataObserver {
public:
    MediaMetadata(uintptr_t rtcEngineHandler);
    static napi_value Init(napi_env env, napi_value exports);
    static void Destructor(napi_env env, void *nativeObject, void *finalize_hint);

private:
    explicit MediaMetadata();
    ~MediaMetadata();

    static napi_value New(napi_env env, napi_callback_info info);
    static napi_value Enable(napi_env env, napi_callback_info info);
    static napi_value Send(napi_env env, napi_callback_info info);

    // IMetadataObserver
    int getMaxMetadataSize() override { return DEFAULT_METADATA_SIZE_IN_BYTE; }
    bool onReadyToSendMetadata(Metadata &metadata, agora::rtc::VIDEO_SOURCE_TYPE source_type) override;
    void onMetadataReceived(const Metadata &metadata) override;

    agora::rtc::IRtcEngine *rtcEngine_;
    napi_env env_;
    napi_ref wrapper_;
    unsigned char* pendingData_ = nullptr;
    int pendingDataSize_ = 0;
};
#endif // APIEXAMPLE_MEDIA_METADATA_H
