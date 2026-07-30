//
// Created on 2024/7/24.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#include "origin_video_data.h"
#include <cassert>
#include <cstring>
#include <vector>

namespace {
struct SnapshotFrame {
    std::vector<uint8_t> data;
    int width;
    int height;
    int rotation;
    int errorCode;
};

void SetInt32Property(napi_env env, napi_value object, const char *name, int value) {
    napi_value property;
    if (napi_create_int32(env, value, &property) == napi_ok) {
        napi_set_named_property(env, object, name, property);
    }
}
} // namespace

OriginVideoData::OriginVideoData(uintptr_t rtcEngineHandler)
    : rtcEngine_(reinterpret_cast<agora::rtc::IRtcEngine *>(rtcEngineHandler)), env_(nullptr), wrapper_(nullptr),
      takeSnapshot_(false), snapshotCallback_(nullptr) {}

OriginVideoData::~OriginVideoData() {
    takeSnapshot_.store(false);
    {
        std::lock_guard<std::mutex> lock(snapshotMutex_);
        if (snapshotCallback_ != nullptr) {
            napi_release_threadsafe_function(snapshotCallback_, napi_tsfn_abort);
            snapshotCallback_ = nullptr;
        }
    }
    if (env_ != nullptr && wrapper_ != nullptr) {
        napi_delete_reference(env_, wrapper_);
    }
}


bool OriginVideoData::onCaptureVideoFrame(agora::rtc::VIDEO_SOURCE_TYPE sourceType, VideoFrame &videoFrame) {
    if (!takeSnapshot_.exchange(false)) {
        return true;
    }

    auto *snapshot = new SnapshotFrame{{}, videoFrame.width, videoFrame.height, videoFrame.rotation, 0};
    const bool invalidFrame = videoFrame.width <= 0 || videoFrame.height <= 0 ||
                              videoFrame.width % 2 != 0 || videoFrame.height % 2 != 0 ||
                              videoFrame.yBuffer == nullptr || videoFrame.uBuffer == nullptr ||
                              videoFrame.vBuffer == nullptr || videoFrame.yStride < videoFrame.width ||
                              videoFrame.uStride < videoFrame.width / 2 || videoFrame.vStride < videoFrame.width / 2;
    if (invalidFrame) {
        snapshot->errorCode = -1;
    } else {
        const int width = videoFrame.width;
        const int height = videoFrame.height;
        const int chromaWidth = width / 2;
        const int chromaHeight = height / 2;
        snapshot->data.resize(width * height + chromaWidth * chromaHeight * 2);

        uint8_t *destination = snapshot->data.data();
        for (int row = 0; row < height; ++row) {
            std::memcpy(destination + row * width, videoFrame.yBuffer + row * videoFrame.yStride, width);
        }

        uint8_t *chromaDestination = destination + width * height;
        for (int row = 0; row < chromaHeight; ++row) {
            const uint8_t *uRow = videoFrame.uBuffer + row * videoFrame.uStride;
            const uint8_t *vRow = videoFrame.vBuffer + row * videoFrame.vStride;
            for (int column = 0; column < chromaWidth; ++column) {
                chromaDestination[row * width + column * 2] = vRow[column];
                chromaDestination[row * width + column * 2 + 1] = uRow[column];
            }
        }
    }

    napi_threadsafe_function callback = nullptr;
    {
        std::lock_guard<std::mutex> lock(snapshotMutex_);
        callback = snapshotCallback_;
        snapshotCallback_ = nullptr;
    }
    if (callback == nullptr) {
        delete snapshot;
        return true;
    }

    napi_status status = napi_call_threadsafe_function(callback, snapshot, napi_tsfn_nonblocking);
    napi_release_threadsafe_function(callback, napi_tsfn_release);
    if (status != napi_ok) {
        delete snapshot;
    }
    return true;
}

void OriginVideoData::CallSnapshotCallback(napi_env env, napi_value callback,
                                           [[maybe_unused]] void *context, void *data) {
    auto *snapshot = static_cast<SnapshotFrame *>(data);
    if (snapshot == nullptr) {
        return;
    }
    if (env == nullptr || callback == nullptr) {
        delete snapshot;
        return;
    }

    napi_value result;
    napi_create_object(env, &result);
    SetInt32Property(env, result, "width", snapshot->width);
    SetInt32Property(env, result, "height", snapshot->height);
    SetInt32Property(env, result, "rotation", snapshot->rotation);
    SetInt32Property(env, result, "errorCode", snapshot->errorCode);

    void *bufferData = nullptr;
    napi_value buffer;
    if (napi_create_arraybuffer(env, snapshot->data.size(), &bufferData, &buffer) == napi_ok) {
        if (!snapshot->data.empty()) {
            std::memcpy(bufferData, snapshot->data.data(), snapshot->data.size());
        }
        napi_set_named_property(env, result, "data", buffer);
    }

    napi_value undefined;
    napi_get_undefined(env, &undefined);
    napi_call_function(env, undefined, callback, 1, &result, nullptr);
    delete snapshot;
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
    assert(napi_define_class(env, "OriginVideoData", NAPI_AUTO_LENGTH, New, nullptr, 2, properties, &cons) == napi_ok);

    assert(napi_create_reference(env, cons, 1, &g_origin_video_data_ref) == napi_ok);
    assert(napi_set_named_property(env, exports, "OriginVideoData", cons) == napi_ok);
    return exports;
}

napi_value OriginVideoData::New(napi_env env, napi_callback_info info) {
    AG_INFO("OriginVideoData::New called");

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

        OriginVideoData *obj = new OriginVideoData(value);

        obj->env_ = env;
        // 通过napi_wrap将ArkTS对象jsThis与C++对象obj绑定
        assert(napi_wrap(env, jsThis, reinterpret_cast<void *>(obj), OriginVideoData::Destructor,
                         nullptr, // finalize_hint
                         &obj->wrapper_) == napi_ok);

        return jsThis;
    } else {
        // 使用`MyObject(...)`调用方式
        size_t argc = 1;
        napi_value args[1];
        assert(napi_get_cb_info(env, info, &argc, args, nullptr, nullptr) == napi_ok && argc == 1);

        napi_value cons;
        assert(napi_get_reference_value(env, g_origin_video_data_ref, &cons) == napi_ok);
        napi_value instance;
        assert(napi_new_instance(env, cons, argc, args, &instance) == napi_ok);

        return instance;
    }
}

napi_value OriginVideoData::Enable(napi_env env, napi_callback_info info) {
    AG_INFO("OriginVideoData::Enable called");

    size_t argc = 1;
    napi_value args[1];
    napi_value jsThis;
    napi_get_cb_info(env, info, &argc, args, &jsThis, nullptr);
    bool enable;
    napi_get_value_bool(env, args[0], &enable);

    OriginVideoData *obj;
    // 通过napi_unwrap将jsThis之前绑定的C++对象取出，并对其进行操作
    assert(napi_unwrap(env, jsThis, reinterpret_cast<void **>(&obj)) == napi_ok);

    int ret = -1;

    agora::media::IMediaEngine *mediaEngine_ = nullptr;
    obj->rtcEngine_->queryInterface(agora::rtc::AGORA_IID_MEDIA_ENGINE, (void **)&mediaEngine_);

    if (enable) {
        ret = mediaEngine_->registerVideoFrameObserver(obj);
    } else {
        ret = mediaEngine_->registerVideoFrameObserver(nullptr);
    }

    napi_value num;
    assert(napi_create_int32(env, ret, &num) == napi_ok);
    return num;
}

napi_value OriginVideoData::TakeSnapshot(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value args[1];
    napi_value jsThis;
    assert(napi_get_cb_info(env, info, &argc, args, &jsThis, nullptr) == napi_ok);

    OriginVideoData *obj;
    // 通过napi_unwrap将jsThis之前绑定的C++对象取出，并对其进行操作
    assert(napi_unwrap(env, jsThis, reinterpret_cast<void **>(&obj)) == napi_ok);

    int result = 0;
    napi_valuetype callbackType = napi_undefined;
    if (argc != 1 || napi_typeof(env, args[0], &callbackType) != napi_ok || callbackType != napi_function) {
        result = -1;
    } else {
        std::lock_guard<std::mutex> lock(obj->snapshotMutex_);
        if (obj->takeSnapshot_.load() || obj->snapshotCallback_ != nullptr) {
            result = -2;
        } else {
            napi_value resourceName;
            napi_create_string_utf8(env, "OriginVideoDataSnapshot", NAPI_AUTO_LENGTH, &resourceName);
            napi_status status = napi_create_threadsafe_function(
                env, args[0], nullptr, resourceName, 1, 1, nullptr, nullptr, nullptr,
                CallSnapshotCallback, &obj->snapshotCallback_);
            if (status == napi_ok) {
                obj->takeSnapshot_.store(true);
            } else {
                obj->snapshotCallback_ = nullptr;
                result = -3;
            }
        }
    }

    napi_value num;
    assert(napi_create_int32(env, result, &num) == napi_ok);
    return num;
}
