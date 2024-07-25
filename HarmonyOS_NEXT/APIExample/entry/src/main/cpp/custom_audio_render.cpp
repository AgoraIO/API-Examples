//
// Created on 2024/7/24.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "custom_audio_render.h"
#include <cassert>

CustomAudioRender::CustomAudioRender(uintptr_t rtcEngineHandler)
    : rtcEngine_(reinterpret_cast<agora::rtc::IRtcEngine *>(rtcEngineHandler)), env_(nullptr), wrapper_(nullptr) {
    audioRender_ = new AudioRenderOHOS();
    audioRender_->init(AudioParameters(48000, 2));
    audioRender_->start();
}

CustomAudioRender::~CustomAudioRender() {
    napi_delete_reference(env_, wrapper_);
    if (audioRender_ != nullptr) {
        audioRender_->stop();
        audioRender_->release();
        delete audioRender_;
        audioRender_ = nullptr;
    }
}

int CustomAudioRender::getObservedAudioFramePosition() {
    return IAudioFrameObserverBase::AUDIO_FRAME_POSITION_PLAYBACK;
}
CustomAudioRender::AudioParams CustomAudioRender::getPlaybackAudioParams() {
    return AudioParams(48000, 2, agora::rtc::RAW_AUDIO_FRAME_OP_MODE_READ_ONLY, 1024);
}
bool CustomAudioRender::onPlaybackAudioFrame(const char *channelId, AudioFrame &audioFrame) {
    AG_INFO("CustomAudioRender::onPlaybackAudioFrame called");
    int audioFrameBufferSize = audioFrame.bytesPerSample * audioFrame.channels * audioFrame.samplesPerChannel;
    if (audioRender_ != nullptr) {
        audioRender_->putAudioData(audioFrame.buffer, audioFrameBufferSize);
    }
    return false;
}


void CustomAudioRender::Destructor(napi_env env, void *nativeObject, [[maybe_unused]] void *finalize_hint) {
    AG_INFO("CustomAudioRender::Destructor called");
    reinterpret_cast<CustomAudioRender *>(nativeObject)->~CustomAudioRender();
}

napi_value CustomAudioRender::Init(napi_env env, napi_value exports) {
    napi_property_descriptor properties[] = {
        {"enable", nullptr, Enable, nullptr, nullptr, nullptr, napi_default, nullptr},
    };

    napi_value cons;
    assert(napi_define_class(env, "CustomAudioRender", NAPI_AUTO_LENGTH, New, nullptr, 1, properties, &cons) ==
           napi_ok);

    assert(napi_create_reference(env, cons, 1, &g_custom_audio_render_ref) == napi_ok);
    assert(napi_set_named_property(env, exports, "CustomAudioRender", cons) == napi_ok);
    return exports;
}

napi_value CustomAudioRender::New(napi_env env, napi_callback_info info) {
    AG_INFO("CustomAudioRender::New called");

    napi_value newTarget;
    assert(napi_get_new_target(env, info, &newTarget) == napi_ok);
    if (newTarget != nullptr) {
        // 使用`new MyObject(...)`调用方式
        size_t argc = 1;
        napi_value args[1];
        napi_value jsThis;
        assert(napi_get_cb_info(env, info, &argc, args, &jsThis, nullptr) == napi_ok);

        uintptr_t value = 0.0;
        bool loss;
        napi_valuetype valuetype;
        assert(napi_typeof(env, args[0], &valuetype) == napi_ok);
        if (valuetype != napi_undefined) {
            assert(napi_get_value_bigint_uint64(env, args[0], &value, &loss) == napi_ok);
        }

        CustomAudioRender *obj = new CustomAudioRender(value);

        obj->env_ = env;
        // 通过napi_wrap将ArkTS对象jsThis与C++对象obj绑定
        assert(napi_wrap(env, jsThis, reinterpret_cast<void *>(obj), CustomAudioRender::Destructor,
                         nullptr, // finalize_hint
                         &obj->wrapper_) == napi_ok);

        return jsThis;
    } else {
        // 使用`MyObject(...)`调用方式
        size_t argc = 1;
        napi_value args[1];
        assert(napi_get_cb_info(env, info, &argc, args, nullptr, nullptr) == napi_ok && argc == 1);

        napi_value cons;
        assert(napi_get_reference_value(env, g_custom_audio_render_ref, &cons) == napi_ok);
        napi_value instance;
        assert(napi_new_instance(env, cons, argc, args, &instance) == napi_ok);

        return instance;
    }
}

napi_value CustomAudioRender::Enable(napi_env env, napi_callback_info info) {
    AG_INFO("CustomAudioRender::Enable called");

    size_t argc = 1;
    napi_value args[1];
    napi_value jsThis;
    napi_get_cb_info(env, info, &argc, args, &jsThis, nullptr);
    bool enable;
    napi_get_value_bool(env, args[0], &enable);

    CustomAudioRender *obj;
    // 通过napi_unwrap将jsThis之前绑定的C++对象取出，并对其进行操作
    assert(napi_unwrap(env, jsThis, reinterpret_cast<void **>(&obj)) == napi_ok);

    int ret = -1;

    agora::media::IMediaEngine *mediaEngine_ = nullptr;
    obj->rtcEngine_->queryInterface(agora::rtc::AGORA_IID_MEDIA_ENGINE, (void **)&mediaEngine_);

    if (enable) {
        ret = mediaEngine_->registerAudioFrameObserver(obj);
    } else {
        ret = mediaEngine_->registerAudioFrameObserver(nullptr);
    }

    napi_value num;
    assert(napi_create_int32(env, ret, &num) == napi_ok);
    return num;
}
