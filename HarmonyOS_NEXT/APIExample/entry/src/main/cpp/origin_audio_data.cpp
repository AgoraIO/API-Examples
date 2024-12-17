//
// Created on 2024/7/24.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "origin_audio_data.h"
#include <cassert>

OriginAudioData::OriginAudioData(uintptr_t rtcEngineHandler)
    : rtcEngine_(reinterpret_cast<agora::rtc::IRtcEngine *>(rtcEngineHandler)), env_(nullptr), wrapper_(nullptr) {}

OriginAudioData::~OriginAudioData() { napi_delete_reference(env_, wrapper_); }

int OriginAudioData::getObservedAudioFramePosition() { return IAudioFrameObserverBase::AUDIO_FRAME_POSITION_RECORD; }
OriginAudioData::AudioParams OriginAudioData::getRecordAudioParams() {
    return AudioParams(48000, 2, agora::rtc::RAW_AUDIO_FRAME_OP_MODE_READ_ONLY, 1024);
}
bool OriginAudioData::onRecordAudioFrame(const char *channelId, AudioFrame &audioFrame) { 
    AG_INFO("OriginAudioData::onRecordAudioFrame called");
    return false; 
}


void OriginAudioData::Destructor(napi_env env, void *nativeObject, [[maybe_unused]] void *finalize_hint) {
    AG_INFO("OriginAudioData::Destructor called");
    reinterpret_cast<OriginAudioData *>(nativeObject)->~OriginAudioData();
}

napi_value OriginAudioData::Init(napi_env env, napi_value exports) {
    napi_property_descriptor properties[] = {
        {"enable", nullptr, Enable, nullptr, nullptr, nullptr, napi_default, nullptr},
    };

    napi_value cons;
    napi_status status1 = napi_define_class(env, "OriginAudioData", NAPI_AUTO_LENGTH, New, nullptr, 1, properties, &cons);
    assert(status1 == napi_ok);

    napi_status status2 = napi_create_reference(env, cons, 1, &g_origin_audio_data_ref);
    assert(status2 == napi_ok);

    napi_status status3 = napi_set_named_property(env, exports, "OriginAudioData", cons);
    assert(status3 == napi_ok);
    return exports;
}

napi_value OriginAudioData::New(napi_env env, napi_callback_info info) {
    AG_INFO("OriginAudioData::New called");

    napi_value newTarget;
    napi_status status1 = napi_get_new_target(env, info, &newTarget);
    assert(status1 == napi_ok);

    if (newTarget != nullptr) {
        // 使用`new MyObject(...)`调用方式
        size_t argc = 1;
        napi_value args[1];
        napi_value jsThis;
        napi_status status2 = napi_get_cb_info(env, info, &argc, args, &jsThis, nullptr);
        assert(status2 == napi_ok);

        uintptr_t value = 0.0;
        bool loss;
        napi_valuetype valuetype;
        napi_status status3 = napi_typeof(env, args[0], &valuetype);
        assert(status3 == napi_ok);

        if (valuetype != napi_undefined) {
            napi_status status4 = napi_get_value_bigint_uint64(env, args[0], &value, &loss);
            assert(status4 == napi_ok);
        }

        OriginAudioData *obj = new OriginAudioData(value);
        obj->env_ = env;
        // 通过napi_wrap将ArkTS对象jsThis与C++对象obj绑定
        napi_status status5 = napi_wrap(env, jsThis, reinterpret_cast<void *>(obj), 
            OriginAudioData::Destructor, nullptr, &obj->wrapper_);
        assert(status5 == napi_ok);

        return jsThis;
    } else {
        // 使用`MyObject(...)`调用方式
        size_t argc = 1;
        napi_value args[1];
        napi_status status6 = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
        assert(status6 == napi_ok && argc == 1);

        napi_value cons;
        napi_status status7 = napi_get_reference_value(env, g_origin_audio_data_ref, &cons);
        assert(status7 == napi_ok);

        napi_value instance;
        napi_status status8 = napi_new_instance(env, cons, argc, args, &instance);
        assert(status8 == napi_ok);

        return instance;
    }
}

napi_value OriginAudioData::Enable(napi_env env, napi_callback_info info) {
    AG_INFO("OriginAudioData::Enable called");

    size_t argc = 1;
    napi_value args[1];
    napi_value jsThis;
    napi_status status1 = napi_get_cb_info(env, info, &argc, args, &jsThis, nullptr);
    assert(status1 == napi_ok);

    bool enable;
    napi_status status2 = napi_get_value_bool(env, args[0], &enable);
    assert(status2 == napi_ok);

    OriginAudioData *obj;
    // 通过napi_unwrap将jsThis之前绑定的C++对象取出，并对其进行操作
    napi_status status3 = napi_unwrap(env, jsThis, reinterpret_cast<void **>(&obj));
    assert(status3 == napi_ok);

    int ret = -1;
    agora::media::IMediaEngine *mediaEngine_ = nullptr;
    obj->rtcEngine_->queryInterface(agora::rtc::AGORA_IID_MEDIA_ENGINE, (void **)&mediaEngine_);

    if (enable) {
        ret = mediaEngine_->registerAudioFrameObserver(obj);
    } else {
        ret = mediaEngine_->registerAudioFrameObserver(nullptr);
    }

    napi_value num;
    napi_status status4 = napi_create_int32(env, ret, &num);
    assert(status4 == napi_ok);
    return num;
}
