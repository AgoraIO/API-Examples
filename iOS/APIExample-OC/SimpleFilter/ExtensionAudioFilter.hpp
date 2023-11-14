//
// Created by 张涛 on 2020/4/26.
//

#ifndef AGORA_EXTENSIONAUDIOFILTER_H
#define AGORA_EXTENSIONAUDIOFILTER_H

#include "AgoraRtcKit/NGIAgoraMediaNode.h"
#include <AgoraRtcKit/AgoraRefCountedObject.h>
#include "AgoraRtcKit/AgoraRefPtr.h"
#include "AudioProcessor.hpp"

namespace agora {
    namespace extension {
        class ExtensionAudioFilter : public agora::rtc::IAudioFilter {
        public:
            ExtensionAudioFilter(const char* name, agora_refptr<AdjustVolumeAudioProcessor> processor);
            ~ExtensionAudioFilter();
            bool adaptAudioFrame(const media::base::AudioPcmFrame& inAudioPcmFrame,
                                 media::base::AudioPcmFrame& adaptedPcmFrame) override;
            void setEnabled(bool enable) override { enabled_ = enable; }
            bool isEnabled() const override { return enabled_; }
            int setProperty(const char* key, const void* buf, int buf_size) override;
            int getProperty(const char* key, void* buf, int buf_size) const override { return ERR_OK; }
            const char* getName() const override { return filterName_.c_str(); }
        private:
            std::atomic_bool enabled_ = {true};
            std::string filterName_;
            agora_refptr<AdjustVolumeAudioProcessor> audioProcessor_;
        protected:
            ExtensionAudioFilter() = default;
        };
    }
}


#endif //AGORA_EXTENSIONAUDIOFILTER_H
