//
// Created by DYF on 2020/7/13.
//

#include "AudioProcessor.h"
#include <chrono>
#include "../logutils.h"

namespace agora {
    namespace extension {
        using limits_int16 = std::numeric_limits<int16_t>;
        int16_t AdjustVolumeAudioProcessor::FloatS16ToS16(float v) {
            static const float kMaxRound = (limits_int16::max)() - 0.5f;
            static const float kMinRound = (limits_int16::min)() + 0.5f;
            if (v > 0) {
                return v >= kMaxRound ? (limits_int16::max)() : static_cast<int16_t>(v + 0.5f);
            }
            return v <= kMinRound ? (limits_int16::min)() : static_cast<int16_t>(v - 0.5f);
        }

        int AdjustVolumeAudioProcessor::processFrame(const media::base::AudioPcmFrame& inAudioPcmFrame,
                                                      media::base::AudioPcmFrame& adaptedPcmFrame) {
//            PRINTF_ERROR("adaptAudioFrame %f", volume_.load());
            size_t length = inAudioPcmFrame.samples_per_channel_ * inAudioPcmFrame.num_channels_;
            for (int idx = 0; idx < length; idx++) {
                adaptedPcmFrame.data_[idx] = FloatS16ToS16(inAudioPcmFrame.data_[idx] * volume_);
            }
            return 0;
        }

        void AdjustVolumeAudioProcessor::dataCallback(const char* data){
            if (control_) {
                control_->postEvent("volume", data);
            }
        }
    }
}
