//
// Created on 2024/6/16.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "napi/native_api.h"
#include "IAgoraRtcEngine.h"
#include "origin_audio_data_observers.h"
#include "utils.h"
static agora::rtc::IRtcEngine *rtcEngine_ = nullptr;
static agora::media::IMediaEngine *mediaEngine_ = nullptr;


napi_value setNativeEngineHandle(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value args[1];
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    uintptr_t handle;
    bool loss;
    napi_get_value_bigint_uint64(env, args[0], &handle, &loss);
    rtcEngine_ = reinterpret_cast<agora::rtc::IRtcEngine *>(handle);
    int result = rtcEngine_->queryInterface(agora::rtc::AGORA_IID_MEDIA_ENGINE, (void **)&mediaEngine_);
    if (result != 0) {
        AG_INFO("get media engine failed :%d ", result);
    }
    return nullptr;
}

napi_value unsetNativeEngineHandle(napi_env env, napi_callback_info info) {
    mediaEngine_ = nullptr;
    rtcEngine_ = nullptr;
    return nullptr;
}


// OriginAudioData
static agora::media::IAudioFrameObserver *audioFrameObserver_ = nullptr;
napi_value enableAudioFrameObserver(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value args[1];
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    bool enable;
    napi_get_value_bool(env, args[0], &enable);
    if (!mediaEngine_)
        return nullptr;
    if (enable) {
        if (audioFrameObserver_ == nullptr) {
            audioFrameObserver_ = new OriginAudioDataObserver();
        }
        mediaEngine_->registerAudioFrameObserver(audioFrameObserver_);
    } else {
        delete audioFrameObserver_;
        audioFrameObserver_ = nullptr;
        mediaEngine_->registerAudioFrameObserver(nullptr);
    }
    return nullptr;
}

// OriginVideoData
napi_value enableVideoFrameObserver(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value args[1];
    napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
    bool enable;
    napi_get_value_bool(env, args[0], &enable);
    if (!mediaEngine_)
        return nullptr;
    return nullptr;
}


EXTERN_C_START
static napi_value Init(napi_env env, napi_value exports) {
    napi_property_descriptor desc[] = {
        {"setNativeEngineHandle", nullptr, setNativeEngineHandle, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"unsetNativeEngineHandle", nullptr, unsetNativeEngineHandle, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"enableAudioFrameObserver", nullptr, enableAudioFrameObserver, nullptr, nullptr, nullptr, napi_default,nullptr},
        {"enableVideoFrameObserver", nullptr, enableVideoFrameObserver, nullptr, nullptr, nullptr, napi_default, nullptr},
    };
    napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
    return exports;
}
EXTERN_C_END

static napi_module demoModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = Init,
    .nm_modname = "rawdataprocessing",
    .nm_priv = ((void *)0),
    .reserved = {0},
};

extern "C" __attribute__((constructor)) void RegisterEntryModule(void) { napi_module_register(&demoModule); }
