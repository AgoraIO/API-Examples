//
// Created by 张涛 on 2020/4/26.
//

#include "ExtensionVideoFilter.h"
#include "../logutils.h"
#include <sstream>

namespace agora {
    namespace extension {

        ExtensionVideoFilter::ExtensionVideoFilter(agora_refptr<WatermarkProcessor> waterMarkProcessor):threadPool_(1) {
            waterMarkProcessor_ = waterMarkProcessor;
        }

        ExtensionVideoFilter::~ExtensionVideoFilter() {
            waterMarkProcessor_->releaseOpenGL();
        }

        //Set the process mode between the SDK and the video plug-in
        //If set ProcessMode to Sync mode, Agora SDK and video extension will pass data through adaptVideoFrame
        //If set ProcessMode to Async mode, Agora SDK and video extension will pass data through pendVideoFrame and deliverVideoFrame
        //If set independent_thread to false, all callbacks sent by Agora SDK are performed on the internal video processing thread
        //If set independent_thread to true, all callbacks sent by Agora SDK are performed on a separate thread
        void ExtensionVideoFilter::getProcessMode(ProcessMode& mode, bool& independent_thread) {
            mode = ProcessMode::kSync;
            independent_thread = false;
            mode_ = mode;
        }

        //Set the type and format of the video data to be processed
        void ExtensionVideoFilter::getVideoFormatWanted(rtc::VideoFrameData::Type& type,
                                                        rtc::RawPixelBuffer::Format& format) {
            type = rtc::VideoFrameData::Type::kRawPixels;
            format = rtc::RawPixelBuffer::Format::kI420;
        }

        int ExtensionVideoFilter::start(agora::agora_refptr<Control> control) {
            PRINTF_INFO("ExtensionVideoFilter::start");
            if (!waterMarkProcessor_) {
                return -1;
            }
            if (control) {
                control_ = control;
                waterMarkProcessor_->setExtensionControl(control);
            }
            if (mode_ == ProcessMode::kAsync){
                invoker_id = threadPool_.RegisterInvoker("thread_videofilter");
                auto res = threadPool_.PostTaskWithRes(invoker_id, [waterMarkProcessor=waterMarkProcessor_] {
                     return waterMarkProcessor->initOpenGL();
                });
                isInitOpenGL = res.get();
            } else {
                isInitOpenGL = waterMarkProcessor_->initOpenGL();
            }
            return 0;
        }

        int ExtensionVideoFilter::stop() {
            PRINTF_INFO("ExtensionVideoFilter::stop");
            if (waterMarkProcessor_) {
                waterMarkProcessor_->releaseOpenGL();
                isInitOpenGL = false;
            }
            return 0;
        }

        rtc::IExtensionVideoFilter::ProcessResult ExtensionVideoFilter::pendVideoFrame(agora::agora_refptr<rtc::IVideoFrame> frame) {
            if (!frame || !isInitOpenGL) {
                return kBypass;
            }

            bool isAsyncMode = (mode_ == ProcessMode::kAsync);
            if (isAsyncMode && waterMarkProcessor_ && control_ && invoker_id >= 0) {
                threadPool_.PostTask(invoker_id, [videoFrame=frame, waterMarkProcessor=waterMarkProcessor_, control=control_] {
                    rtc::VideoFrameData srcData;
                    videoFrame->getVideoFrameData(srcData);
                    waterMarkProcessor->processFrame(srcData);
                    // In asynchronous mode (mode is set to Async),
                    // the plug-in needs to call this method to return the processed video frame to the SDK.
                    control->deliverVideoFrame(videoFrame);
                });
                return kSuccess;
            }
            return kBypass;
        }

        rtc::IExtensionVideoFilter::ProcessResult ExtensionVideoFilter::adaptVideoFrame(agora::agora_refptr<rtc::IVideoFrame> src,
                                                                               agora::agora_refptr<rtc::IVideoFrame>& dst) {
            if (!isInitOpenGL) {
                return kBypass;
            }
            bool isSyncMode = (mode_ == ProcessMode::kSync);
            if (isSyncMode && waterMarkProcessor_) {
                rtc::VideoFrameData srcData;
                src->getVideoFrameData(srcData);
                waterMarkProcessor_->processFrame(srcData);
                dst = src;
                return kSuccess;
            }
            return kBypass;
        }

        // When the app developer calls RtcEngine.setExtensionProperty,
        // Agora SDK will call this method to set video plug-in properties
        int ExtensionVideoFilter::setProperty(const char *key, const void *buf,
                                                 size_t buf_size) {
            PRINTF_INFO("setProperty  %s  %p", key, buf);
            std::string stringParameter((char*)buf);
            waterMarkProcessor_->setParameters(stringParameter);
            return 0;
        }

        // When the app developer calls getExtensionProperty,
        // Agora SDK will call this method to get the properties of the video plug-in
        int ExtensionVideoFilter::getProperty(const char *key, void *buf, size_t buf_size) {
            return -1;
        }

        void ExtensionVideoFilter::setEnabled(bool enable) {
        }

        bool ExtensionVideoFilter::isEnabled() {
            return true;
        }
    }
}
