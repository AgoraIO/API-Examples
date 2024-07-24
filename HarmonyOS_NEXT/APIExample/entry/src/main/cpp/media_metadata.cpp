//
// Created on 2024/7/24.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "media_metadata.h"
#include <cassert>

MediaMetadata::MediaMetadata(uintptr_t rtcEngineHandler)
    : rtcEngine_(reinterpret_cast<agora::rtc::IRtcEngine *>(rtcEngineHandler)), env_(nullptr), wrapper_(nullptr) {}

MediaMetadata::~MediaMetadata() { napi_delete_reference(env_, wrapper_); }

bool MediaMetadata::onReadyToSendMetadata(Metadata &metadata, agora::rtc::VIDEO_SOURCE_TYPE source_type) {
    if(pendingData_){
        memcpy(metadata.buffer, pendingData_, pendingDataSize_);
        metadata.size = pendingDataSize_;
        AG_INFO("MediaMetadata::onReadyToSendMetadata send -- data=%{public}s, size=%{public}d", pendingData_, pendingDataSize_);
        delete [] pendingData_;
        pendingData_ = nullptr;
        pendingDataSize_ = 0;
        return true;
    }
    return false;
}

void MediaMetadata::onMetadataReceived(const Metadata &metadata) {
    AG_INFO("MediaMetadata::onMetadataReceived -- data=%{public}s", metadata.buffer);
}

void MediaMetadata::Destructor(napi_env env, void *nativeObject, [[maybe_unused]] void *finalize_hint) {
    AG_INFO("MediaMetadata::Destructor called");
    reinterpret_cast<MediaMetadata *>(nativeObject)->~MediaMetadata();
}

napi_value MediaMetadata::Init(napi_env env, napi_value exports) {
    napi_property_descriptor properties[] = {
        {"enable", nullptr, Enable, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"send", nullptr, Send, nullptr, nullptr, nullptr, napi_default, nullptr},
    };

    napi_value cons;
    assert(napi_define_class(env, "MediaMetadata", NAPI_AUTO_LENGTH, New, nullptr, 2, properties, &cons) == napi_ok);

    assert(napi_create_reference(env, cons, 1, &g_media_metadata_ref) == napi_ok);
    assert(napi_set_named_property(env, exports, "MediaMetadata", cons) == napi_ok);
    return exports;
}

napi_value MediaMetadata::New(napi_env env, napi_callback_info info) {
    AG_INFO("MediaMetadata::New called");

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

        MediaMetadata *obj = new MediaMetadata(value);

        obj->env_ = env;
        // 通过napi_wrap将ArkTS对象jsThis与C++对象obj绑定
        assert(napi_wrap(env, jsThis, reinterpret_cast<void *>(obj), MediaMetadata::Destructor,
                         nullptr, // finalize_hint
                         &obj->wrapper_) == napi_ok);

        return jsThis;
    } else {
        // 使用`MyObject(...)`调用方式
        size_t argc = 1;
        napi_value args[1];
        assert(napi_get_cb_info(env, info, &argc, args, nullptr, nullptr) == napi_ok && argc == 1);

        napi_value cons;
        assert(napi_get_reference_value(env, g_media_metadata_ref, &cons) == napi_ok);
        napi_value instance;
        assert(napi_new_instance(env, cons, argc, args, &instance) == napi_ok);

        return instance;
    }
}

napi_value MediaMetadata::Enable(napi_env env, napi_callback_info info) {
    AG_INFO("MediaMetadata::Enable called");

    size_t argc = 1;
    napi_value args[1];
    napi_value jsThis;
    napi_get_cb_info(env, info, &argc, args, &jsThis, nullptr);
    bool enable;
    napi_get_value_bool(env, args[0], &enable);

    MediaMetadata *obj;
    // 通过napi_unwrap将jsThis之前绑定的C++对象取出，并对其进行操作
    assert(napi_unwrap(env, jsThis, reinterpret_cast<void **>(&obj)) == napi_ok);

    int ret = -1;

    if (enable) {
        ret = obj->rtcEngine_->registerMediaMetadataObserver(obj, IMetadataObserver::METADATA_TYPE::VIDEO_METADATA);
    } else {
        ret = obj->rtcEngine_->unregisterMediaMetadataObserver(obj, IMetadataObserver::METADATA_TYPE::VIDEO_METADATA);
    }

    napi_value num;
    assert(napi_create_int32(env, ret, &num) == napi_ok);
    return num;
}

napi_value MediaMetadata::Send(napi_env env, napi_callback_info info) {

    size_t argc = 1;
    napi_value args[1];
    napi_value jsThis;
    napi_get_cb_info(env, info, &argc, args, &jsThis, nullptr);
    char buf[DEFAULT_METADATA_SIZE_IN_BYTE] = {0};
    size_t size;
    napi_get_value_string_utf8(env, args[0], buf, DEFAULT_METADATA_SIZE_IN_BYTE, &size);

    MediaMetadata *obj;
    // 通过napi_unwrap将jsThis之前绑定的C++对象取出，并对其进行操作
    assert(napi_unwrap(env, jsThis, reinterpret_cast<void **>(&obj)) == napi_ok);

    if (!obj->pendingData_) {
        obj->pendingData_ = new unsigned char[DEFAULT_METADATA_SIZE_IN_BYTE]{0};
    }
    memset(obj->pendingData_, 0, DEFAULT_METADATA_SIZE_IN_BYTE);
    memcpy(obj->pendingData_, buf, size);
    obj->pendingDataSize_ = size;


    napi_value num;
    assert(napi_create_int32(env, 0, &num) == napi_ok);
    return num;
}
