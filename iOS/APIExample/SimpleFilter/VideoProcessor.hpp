//
// Created by DYF on 2020/7/13.
//

#ifndef AGORA_VIDEOPROCESSOR_H
#define AGORA_VIDEOPROCESSOR_H

#include <thread>
#include <string>
#include <mutex>
#include <vector>
#include <AgoraRtcKit/AgoraRefPtr.h>
#include "AgoraRtcKit/NGIAgoraMediaNode.h"
#include "AgoraRtcKit/AgoraMediaBase.h"
#include "AgoraRtcKit/AgoraExtensions.h"

namespace agora {
    namespace extension {
        class YUVImageProcessor  : public RefCountInterface {
        public:
            bool initOpenGL();

            bool releaseOpenGL();

            int processFrame(agora::rtc::VideoFrameData &capturedFrame);

            int setParameters(std::string parameter);

            std::thread::id getThreadId();

            int setExtensionControl(agora::agora_refptr<rtc::IExtensionVideoFilter::Control> control){
                control_ = control;
                return 0;
            };

        protected:
            ~YUVImageProcessor() {}
        private:
            void process(const agora::rtc::VideoFrameData &capturedFrame);
            void dataCallback(const char* data);
            
            std::mutex mutex_;
            agora::agora_refptr<rtc::IExtensionVideoFilter::Control> control_;
            bool wmEffectEnabled_ = true;
            std::string wmStr_= "Agora";
        };
    }
}


#endif //AGORA_VIDEOPROCESSOR_H
