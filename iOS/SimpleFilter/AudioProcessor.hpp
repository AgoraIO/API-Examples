//
// Created by DYF on 2020/7/13.
//

#ifndef AGORAWITHBYTEDANCE_AUDIOPROCESSOR_H
#define AGORAWITHBYTEDANCE_AUDIOPROCESSOR_H

#include <thread>
#include <string>
#include <mutex>
#include <vector>
#include <AgoraRtcKit/AgoraRefPtr.h>
#include "AgoraRtcKit/NGIAgoraMediaNode.h"
#include "AgoraRtcKit/AgoraMediaBase.h"

namespace agora {
    namespace extension {
        class AdjustVolumeAudioProcessor  : public RefCountInterface {
        public:
            int processFrame(const agora::media::base::AudioPcmFrame &audioPcmFrame,
                             media::base::AudioPcmFrame& adaptedPcmFrame);

            void dataCallback(const char* data);

            void setVolume(int volume) { volume_ = volume / 100.0f; }

            int setExtensionControl(agora::agora_refptr<rtc::IExtensionVideoFilter::Control> control){
                control_ = control;
                return 0;
            };
        protected:
            ~AdjustVolumeAudioProcessor() {}
        private:
            static int16_t FloatS16ToS16(float v);
        private:
            std::atomic<float> volume_ = {1.0f};
            agora::agora_refptr<rtc::IExtensionVideoFilter::Control> control_;
        };
    }
}


#endif //AGORAWITHBYTEDANCE_AUDIOPROCESSOR_H
