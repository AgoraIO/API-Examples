//
// Created on 2024/7/24.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "origin_video_data.h"
#include <cassert>

OriginVideoData::OriginVideoData(uintptr_t rtcEngineHandler)
    : rtcEngine_(reinterpret_cast<agora::rtc::IRtcEngine *>(rtcEngineHandler)), env_(nullptr), wrapper_(nullptr),takeSnapshot_(false) {}

OriginVideoData::~OriginVideoData() { napi_delete_reference(env_, wrapper_); }


bool OriginVideoData::onCaptureVideoFrame(agora::rtc::VIDEO_SOURCE_TYPE sourceType, VideoFrame &videoFrame) {
    if (takeSnapshot_) {
        uint8_t *buffer = videoFrame.yBuffer;
        // real path: /data/app/el2/100/base/io.agora.hmos.apiexample/haps/entry/files/capture.i420
        // play file: ffplay -f rawvideo -pixel_format yuv420p -video_size 1280x720 /Users/xcz/Downloads/capture.i420
        SaveI420Buffer(buffer, videoFrame.width, videoFrame.height,
                       "/data/storage/el2/base/haps/entry/files/capture.i420");
        takeSnapshot_ = false;
    }
    return true;
}

void OriginVideoData::SaveI420Buffer(const uint8_t *buf, int width, int height, std::string filename) {
    AG_INFO("OriginVideoData::SaveI420Buffer takeSnapshot -- width=%{public}d, height=%{public}d, filename=%{public}s", width, height,
            filename.c_str());

    FILE *dump_file_ = fopen(filename.c_str(), "w");
    if (!dump_file_) {
        return;
    }
    int size = width * height * 3 / 2;
    fwrite(buf, sizeof(uint8_t), size, dump_file_);
    fflush(dump_file_);
    fclose(dump_file_);
}

OriginVideoData::VIDEO_FRAME_PROCESS_MODE OriginVideoData::getVideoFrameProcessMode() { return PROCESS_MODE_READ_ONLY; }

agora::media::base::VIDEO_PIXEL_FORMAT OriginVideoData::getVideoFormatPreference() {
    return agora::media::base::VIDEO_PIXEL_I420;
}

uint32_t OriginVideoData::getObservedFramePosition() { return agora::media::base::POSITION_POST_CAPTURER; }


void OriginVideoData::Destructor(napi_env env, void *nativeObject, [[maybe_unused]] void *finalize_hint) {
    AG_INFO("OriginVideoData::Destructor called");
    reinterpret_cast<OriginVideoData *>(nativeObject)->~OriginVideoData();
}

napi_value OriginVideoData::Init(napi_env env, napi_value exports) {
    napi_property_descriptor properties[] = {
        {"enable", nullptr, Enable, nullptr, nullptr, nullptr, napi_default, nullptr},
        {"takeSnapshot", nullptr, TakeSnapshot, nullptr, nullptr, nullptr, napi_default, nullptr},
    };

    napi_value cons;
    napi_status status1 = napi_define_class(env, "OriginVideoData", NAPI_AUTO_LENGTH, New, nullptr, 2, properties, &cons);
    assert(status1 == napi_ok);

    napi_status status2 = napi_create_reference(env, cons, 1, &g_origin_video_data_ref);
    assert(status2 == napi_ok);

    napi_status status3 = napi_set_named_property(env, exports, "OriginVideoData", cons);
    assert(status3 == napi_ok);
    return exports;
}

napi_value OriginVideoData::New(napi_env env, napi_callback_info info) {
    AG_INFO("OriginVideoData::New called");

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

        OriginVideoData *obj = new OriginVideoData(value);
        obj->env_ = env;
        // 通过napi_wrap将ArkTS对象jsThis与C++对象obj绑定
        napi_status status5 = napi_wrap(env, jsThis, reinterpret_cast<void *>(obj), 
            OriginVideoData::Destructor, nullptr, &obj->wrapper_);
        assert(status5 == napi_ok);

        return jsThis;
    } else {
        // 使用`MyObject(...)`调用方式
        size_t argc = 1;
        napi_value args[1];
        napi_status status6 = napi_get_cb_info(env, info, &argc, args, nullptr, nullptr);
        assert(status6 == napi_ok && argc == 1);

        napi_value cons;
        napi_status status7 = napi_get_reference_value(env, g_origin_video_data_ref, &cons);
        assert(status7 == napi_ok);

        napi_value instance;
        napi_status status8 = napi_new_instance(env, cons, argc, args, &instance);
        assert(status8 == napi_ok);

        return instance;
    }
}

napi_value OriginVideoData::Enable(napi_env env, napi_callback_info info) {
    AG_INFO("OriginVideoData::Enable called");

    size_t argc = 1;
    napi_value args[1];
    napi_value jsThis;
    napi_status status1 = napi_get_cb_info(env, info, &argc, args, &jsThis, nullptr);
    assert(status1 == napi_ok);

    bool enable;
    napi_status status2 = napi_get_value_bool(env, args[0], &enable);
    assert(status2 == napi_ok);

    OriginVideoData *obj;
    // 通过napi_unwrap将jsThis之前绑定的C++对象取出，并对其进行操作
    napi_status status3 = napi_unwrap(env, jsThis, reinterpret_cast<void **>(&obj));
    assert(status3 == napi_ok);

    int ret = -1;
    agora::media::IMediaEngine *mediaEngine_ = nullptr;
    obj->rtcEngine_->queryInterface(agora::rtc::AGORA_IID_MEDIA_ENGINE, (void **)&mediaEngine_);

    if (enable) {
        ret = mediaEngine_->registerVideoFrameObserver(obj);
    } else {
        ret = mediaEngine_->registerVideoFrameObserver(nullptr);
    }

    napi_value num;
    napi_status status4 = napi_create_int32(env, ret, &num);
    assert(status4 == napi_ok);
    return num;
}

napi_value OriginVideoData::TakeSnapshot(napi_env env, napi_callback_info info) {
    napi_value jsThis;
    napi_status status1 = napi_get_cb_info(env, info, nullptr, nullptr, &jsThis, nullptr);
    assert(status1 == napi_ok);

    OriginVideoData *obj;
    // 通过napi_unwrap将jsThis之前绑定的C++对象取出，并对其进行操作
    napi_status status2 = napi_unwrap(env, jsThis, reinterpret_cast<void **>(&obj));
    assert(status2 == napi_ok);

    obj->takeSnapshot_ = true;

    napi_value num;
    napi_status status3 = napi_create_int32(env, 0, &num);
    assert(status3 == napi_ok);
    return num;
}
