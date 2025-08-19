//
// Created by 张涛 on 2020/4/26.
//

#include "ExtensionAudioFilter.h"
#include "../logutils.h"
#include <sstream>

namespace agora {
    namespace extension {
        ExtensionAudioFilter::ExtensionAudioFilter(const char* name, agora_refptr<AdjustVolumeAudioProcessor> audioProcessor) {
            PRINTF_INFO("ExtensionAudioFilter 构造: name=%s, audioProcessor_=%p", name, audioProcessor.get());
            filterName_ = name;
            audioProcessor_ = audioProcessor;
        }

        ExtensionAudioFilter::~ExtensionAudioFilter() {

        }

        bool ExtensionAudioFilter::adaptAudioFrame(const media::base::AudioPcmFrame& inAudioPcmFrame,
                                                      media::base::AudioPcmFrame& adaptedPcmFrame) {
            PRINTF_INFO("ExtensionAudioFilter adaptAudioFrame: filterName=%s, audioProcessor_=%p", filterName_.c_str(), audioProcessor_.get());
            return audioProcessor_->processFrame(inAudioPcmFrame, adaptedPcmFrame) == 0;
        }

        int ExtensionAudioFilter::setProperty(const char* key, const void* buf, int buf_size) {
            PRINTF_INFO("ExtensionAudioFilter setProperty: key=%s, buf=%p, buf_size=%d", key, buf, buf_size);
            PRINTF_INFO("ExtensionAudioFilter setProperty  %s  %p", key, buf);
            std::string str_volume = "100";
            if (std::string(key) == "volume") {
                str_volume = std::string(static_cast<const char*>(buf), buf_size);
            }

            int int_volume_ = atoi(str_volume.c_str());
            audioProcessor_->setVolume(int_volume_);
            return ERR_OK;
        }
    }
}
