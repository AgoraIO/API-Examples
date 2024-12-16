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
    napi_status status1 = napi_define_class(env, "MediaMetadata", NAPI_AUTO_LENGTH, New, nullptr, 2, properties, &cons);
    assert(status1 == napi_ok);

    napi_status status2 = napi_create_reference(env, cons, 1, &g_media_metadata_ref);
    assert(status2 == napi_ok);

    napi_status status3 = napi_set_named_property(env, exports, "MediaMetadata", cons);
    assert(status3 == napi_ok);
    return exports;
}

napi_value MediaMetadata::New(napi_env env, napi_callback_info info) {
    AG_INFO("MediaMetadata::New called");

    napi_value newTarget;
    napi_status status1 = napi_get_new_target(env, info, &newTarget);
    assert(status1 == napi_ok);

    if (newTarget != nullptr) {
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

        MediaMetadata *obj = new MediaMetadata(value);
        obj->env_ = env;

        napi_status status5 = napi_wrap(env, jsThis, reinterpret_cast<void *>(obj), 
            MediaMetadata::Destructor, nullptr, &obj->wrapper_);
        assert(status5 == napi_ok);

        return jsThis;
    } else {
        size_t argc = 1;
        napi_value args[1];
        napi_status status6 = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
        assert(status6 == napi_ok && argc == 1);

        napi_value cons;
        napi_status status7 = napi_get_reference_value(env, g_media_metadata_ref, &cons);
        assert(status7 == napi_ok);

        napi_value instance;
        napi_status status8 = napi_new_instance(env, cons, argc, args, &instance);
        assert(status8 == napi_ok);

        return instance;
    }
}

napi_value MediaMetadata::Enable(napi_env env, napi_callback_info info) {
    AG_INFO("MediaMetadata::Enable called");

    size_t argc = 1;
    napi_value args[1];
    napi_value jsThis;
    napi_status status1 = napi_get_cb_info(env, info, &argc, args, &jsThis, nullptr);
    assert(status1 == napi_ok);

    bool enable;
    napi_status status2 = napi_get_value_bool(env, args[0], &enable);
    assert(status2 == napi_ok);

    MediaMetadata *obj;
    napi_status status3 = napi_unwrap(env, jsThis, reinterpret_cast<void **>(&obj));
    assert(status3 == napi_ok);

    int ret = -1;
    if (enable) {
        ret = obj->rtcEngine_->registerMediaMetadataObserver(obj, IMetadataObserver::METADATA_TYPE::VIDEO_METADATA);
    } else {
        ret = obj->rtcEngine_->unregisterMediaMetadataObserver(obj, IMetadataObserver::METADATA_TYPE::VIDEO_METADATA);
    }

    napi_value num;
    napi_status status4 = napi_create_int32(env, ret, &num);
    assert(status4 == napi_ok);
    return num;
}

napi_value MediaMetadata::Send(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value args[1];
    napi_value jsThis;
    napi_status status1 = napi_get_cb_info(env, info, &argc, args, &jsThis, nullptr);
    assert(status1 == napi_ok);

    char buf[DEFAULT_METADATA_SIZE_IN_BYTE] = {0};
    size_t size;
    napi_status status2 = napi_get_value_string_utf8(env, args[0], buf, DEFAULT_METADATA_SIZE_IN_BYTE, &size);
    assert(status2 == napi_ok);

    MediaMetadata *obj;
    napi_status status3 = napi_unwrap(env, jsThis, reinterpret_cast<void **>(&obj));
    assert(status3 == napi_ok);

    if (!obj->pendingData_) {
        obj->pendingData_ = new unsigned char[DEFAULT_METADATA_SIZE_IN_BYTE]{0};
    }
    memset(obj->pendingData_, 0, DEFAULT_METADATA_SIZE_IN_BYTE);
    memcpy(obj->pendingData_, buf, size);
    obj->pendingDataSize_ = size;

    napi_value num;
    napi_status status4 = napi_create_int32(env, 0, &num);
    assert(status4 == napi_ok);
    return num;
}
