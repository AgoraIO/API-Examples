//
// Created by DYF on 2020/7/13.
//

#ifndef AGORAWITHBYTEDANCE_VIDEOPROCESSOR_H
#define AGORAWITHBYTEDANCE_VIDEOPROCESSOR_H

#include <thread>
#include <string>
#include <mutex>
#include <vector>
#include <AgoraRtcKit/AgoraRefPtr.h>
#include "AgoraRtcKit/NGIAgoraMediaNode.h"

#include "AgoraRtcKit/AgoraMediaBase.h"
#include "opencv2/core/utility.hpp"

#include "EGLCore.h"
#include "rapidjson/rapidjson.h"
#include "AgoraRtcKit/AgoraExtensions.h"

namespace agora {
    namespace extension {
        class WatermarkProcessor  : public RefCountInterface {
        public:
            bool initOpenGL();

            bool releaseOpenGL();

            bool makeCurrent();

            bool detachCurrent();

            int processFrame(agora::rtc::VideoFrameData &capturedFrame);

            int setParameters(std::string parameter);

            std::thread::id getThreadId();

            int setExtensionControl(agora::agora_refptr<rtc::IExtensionVideoFilter::Control> control){
                control_ = control;
                return 0;
            };

        protected:
            ~WatermarkProcessor() {}
        private:
            void addWatermark(const agora::rtc::VideoFrameData &capturedFrame);
            void dataCallback(const char* data);

#if defined(__ANDROID__) || defined(TARGET_OS_ANDROID)
            EglCore *eglCore_ = nullptr;
            EGLSurface offscreenSurface_ = nullptr;
#endif
            std::mutex mutex_;
            agora::agora_refptr<rtc::IExtensionVideoFilter::Control> control_;
            bool wmEffectEnabled_ = false;
            std::string wmStr_= "agora";
        };
    }
}


#endif //AGORAWITHBYTEDANCE_VIDEOPROCESSOR_H
