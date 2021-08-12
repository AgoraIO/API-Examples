//
// Created by DYF on 2020/7/13.
//

#include "VideoProcessor.hpp"

#include <chrono>

#include <opencv2/opencv.hpp>

namespace agora {
    namespace extension {
        bool WatermarkProcessor::initOpenGL() {
            const std::lock_guard<std::mutex> lock(mutex_);
            return true;
        }

        bool WatermarkProcessor::releaseOpenGL() {
            const std::lock_guard<std::mutex> lock(mutex_);
            return true;
        }

        int WatermarkProcessor::processFrame(agora::rtc::VideoFrameData &capturedFrame) {
            if (wmEffectEnabled_) {
                addWatermark(capturedFrame);
            }
            return 0;
        }

        void WatermarkProcessor::addWatermark(const agora::rtc::VideoFrameData &capturedFrame) {
            cv::Mat image(capturedFrame.height, capturedFrame.width, CV_8U,
                           (void*)capturedFrame.pixels.data);
            double fontSize = image.cols / 400.0;
            cv::Point point(image.cols/2, image.rows/2);
            cv::Scalar scalar(255, 0, 0);
            cv::Mat textImg = cv::Mat::zeros(image.rows, image.cols, image.type());
            cv::putText(textImg, wmStr_, point, cv::FONT_HERSHEY_DUPLEX, fontSize, scalar, 3);
            cv::flip(textImg, textImg, 1);
            image = image + textImg;
        }

        int WatermarkProcessor::setParameters(std::string parameter) {
            const std::lock_guard<std::mutex> lock(mutex_);
            
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
