//
// Created by DYF on 2020/7/13.
//

#include "VideoProcessor.hpp"

#include <chrono>

namespace agora {
    namespace extension {
    bool enableGrey = true;
        bool YUVImageProcessor::initOpenGL() {
            const std::lock_guard<std::mutex> lock(mutex_);
            return true;
        }

        bool YUVImageProcessor::releaseOpenGL() {
            const std::lock_guard<std::mutex> lock(mutex_);
            return true;
        }

        int YUVImageProcessor::processFrame(agora::rtc::VideoFrameData &capturedFrame) {
            if (wmEffectEnabled_) {
                process(capturedFrame);
            }
            return 0;
        }

        void YUVImageProcessor::process(const agora::rtc::VideoFrameData &capturedFrame) {
            if(!enableGrey){
                return;
            }
//            agora::rtc::RawPixelBuffer::Format format = capturedFrame.pixels.format;
            unsigned char* pic = capturedFrame.pixels.data;
            int offset = capturedFrame.height * capturedFrame.width;
            memset(pic + offset, 128, capturedFrame.height * capturedFrame.width / 2);
        }

        int YUVImageProcessor::setParameters(std::string parameter) {
            const std::lock_guard<std::mutex> lock(mutex_);
            if(parameter == "1"){
                enableGrey = true;
            }
            else{
                enableGrey = false;
            }
            return 0;
        }

        std::thread::id YUVImageProcessor::getThreadId() {
            std::thread::id id = std::this_thread::get_id();
            return id;
        }

        void YUVImageProcessor::dataCallback(const char* data){
            if (control_) {
                control_->postEvent("key", data);
            }
        }
    }
}
