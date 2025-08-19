//
// Created by 张涛 on 2020/4/26.
//

#ifndef AGORAWITHBYTEDANCE_EXTENSION_PROVIDER_H
#define AGORAWITHBYTEDANCE_EXTENSION_PROVIDER_H

#include "AgoraRtcKit/NGIAgoraExtensionProvider.h"
#include "ExtensionVideoFilter.h"
#include "ExtensionAudioFilter.h"

namespace agora {
    namespace extension {
        static const char* VIDEO_FILTER_NAME = "Watermark";
        static const char* AUDIO_FILTER_NAME = "VolumeChange";

        class ExtensionProvider : public agora::rtc::IExtensionProvider {
        private:
            agora_refptr<WatermarkProcessor> waterMarkProcessor_;
            agora_refptr<AdjustVolumeAudioProcessor> audioProcessor_;
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
#endif //AGORAWITHBYTEDANCE_EXTENSION_PROVIDER_H
