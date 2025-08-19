//
// Created by 张涛 on 2020/4/26.
//

#ifndef AGORA_EXTENSION_PROVIDER_H
#define AGORA_EXTENSION_PROVIDER_H

#include "AgoraRtcKit/NGIAgoraExtensionProvider.h"
#include "ExtensionAudioFilter.hpp"
#include "ExtensionVideoFilter.hpp"

namespace agora {
    namespace extension {
        static const char* AUDIO_FILTER_NAME = "VolumeChange";
        static const char* VIDEO_FILTER_NAME = "Grey";

        class ExtensionProvider : public agora::rtc::IExtensionProvider {
        private:
            agora_refptr<AdjustVolumeAudioProcessor> audioProcessor_;
            agora_refptr<YUVImageProcessor> YUVProcessor_;
        public:
            ExtensionProvider();

            ~ExtensionProvider();

            void setExtensionControl(rtc::IExtensionControl* control) override;
            void enumerateExtensions(ExtensionMetaInfo* extension_list, int& extension_count) override;
            agora_refptr<rtc::IAudioFilter> createAudioFilter(const char* name) override;
            agora_refptr<rtc::IExtensionVideoFilter> createVideoFilter(const char* name) override;
            agora_refptr<rtc::IExtensionVideoSink> createVideoSink(const char* name) override;
        };
    }
}
#endif //AGORA_EXTENSION_PROVIDER_H
