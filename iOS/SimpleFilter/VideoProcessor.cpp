//
// Created by DYF on 2020/7/13.
//

#include "VideoProcessor.hpp"

#include <chrono>


#define CHECK_BEF_AI_RET_SUCCESS(ret, ...) \
if(ret != 0){\
    PRINTF_ERROR(__VA_ARGS__);\
}

namespace agora {
    namespace extension {
        bool WatermarkProcessor::initOpenGL() {
            const std::lock_guard<std::mutex> lock(mutex_);
            return true;
        }

        bool WatermarkProcessor::releaseOpenGL() {
            const std::lock_guard<std::mutex> lock(mutex_);

#if defined(__ANDROID__) || defined(TARGET_OS_ANDROID)
            if (eglCore_) {
                if (offscreenSurface_) {
                    eglCore_->releaseSurface(offscreenSurface_);
                }
                delete eglCore_;
                eglCore_ = nullptr;
            }
#endif
            return true;
        }

        int WatermarkProcessor::processFrame(agora::rtc::VideoFrameData &capturedFrame) {
//            PRINTF_INFO("processFrame: w: %d,  h: %d,  r: %d, enable: %d", capturedFrame.width, capturedFrame.height, capturedFrame.rotation, wmEffectEnabled_);
            if (wmEffectEnabled_) {
                addWatermark(capturedFrame);
            }
            return 0;
        }

        void WatermarkProcessor::addWatermark(const agora::rtc::VideoFrameData &capturedFrame) {
//            cv::Mat image(capturedFrame.height, capturedFrame.width, CV_8U,
//                           (void*)capturedFrame.pixels.data);
//            double fontSize = image.cols / 800;
//            if (fontSize == 0.0) {
//                fontSize = 2.0;
//            }
//            cv::Point point(image.rows/2, image.cols/2);
//            cv::Scalar scalar(255, 0, 0);
//            cv::Mat textImg = cv::Mat::zeros(image.rows, image.cols, image.type());
//            cv::putText(textImg, wmStr_, point, cv::FONT_HERSHEY_DUPLEX, fontSize, scalar,3);
//            cv::flip(textImg, textImg, 0);
//            image = image + textImg;
        }

        int WatermarkProcessor::setParameters(std::string parameter) {
            const std::lock_guard<std::mutex> lock(mutex_);
//            Document d;
//            d.Parse(parameter.c_str());
//            if (d.HasParseError()) {
//                return -ERROR_INVALID_JSON;
//            }
//
//
//            if (d.HasMember("plugin.watermark.wmEffectEnabled")) {
//                Value& enabled = d["plugin.watermark.wmEffectEnabled"];
//                if (!enabled.IsBool()) {
//                    return -ERROR_INVALID_JSON_TYPE;
//                }
//                wmEffectEnabled_ = enabled.GetBool();
//            }
//
//            if (d.HasMember("plugin.watermask.wmStr")) {
//                Value& wmStr = d["plugin.watermask.wmStr"];
//                if (!wmStr.IsString()) {
//                    return -ERROR_INVALID_JSON_TYPE;
//                }
//                wmStr_ = wmStr.GetString();
//            }

            return 0;
        }

        std::thread::id WatermarkProcessor::getThreadId() {
            std::thread::id id = std::this_thread::get_id();
            return id;
        }

        void WatermarkProcessor::dataCallback(const char* data){
            if (control_) {
                control_->postEvent("key", data);
            }
        }
    }
}
