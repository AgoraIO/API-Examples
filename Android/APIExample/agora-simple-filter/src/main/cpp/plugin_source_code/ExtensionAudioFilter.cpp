//
// Created by 张涛 on 2020/4/26.
//

#include "ExtensionAudioFilter.h"
#include "../logutils.h"
#include <sstream>

namespace agora {
    namespace extension {
        ExtensionAudioFilter::ExtensionAudioFilter(const char* name, agora_refptr<AdjustVolumeAudioProcessor> audioProcessor) {
            filterName_ = name;
            audioProcessor_ = audioProcessor;
        }

        ExtensionAudioFilter::~ExtensionAudioFilter() {

        }

        bool ExtensionAudioFilter::adaptAudioFrame(const media::base::AudioPcmFrame& inAudioPcmFrame,
                                                      media::base::AudioPcmFrame& adaptedPcmFrame) {
            return audioProcessor_->processFrame(inAudioPcmFrame, adaptedPcmFrame) == 0;
        }

        int ExtensionAudioFilter::setProperty(const char* key, const void* buf, int buf_size) {
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
