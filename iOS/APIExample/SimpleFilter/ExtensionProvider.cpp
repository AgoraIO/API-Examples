//
// Created by 张涛 on 2020/4/26.
//

#include "ExtensionProvider.hpp"

namespace agora {
    namespace extension {
        ExtensionProvider::ExtensionProvider() {
            audioProcessor_ = new agora::RefCountedObject<AdjustVolumeAudioProcessor>();
            YUVProcessor_ = new agora::RefCountedObject<YUVImageProcessor>();
        }

        ExtensionProvider::~ExtensionProvider() {
            audioProcessor_.reset();
            YUVProcessor_.reset();
        }

        // Provide information about all plug-ins that support packaging.
        // When the SDK loads the plug-in, it will call this method to send a callback to the plug-in.
        // You need to provide information about all plug-ins that support encapsulation.
        void ExtensionProvider::enumerateExtensions(ExtensionMetaInfo* extension_list,
                                                           int& extension_count) {
            extension_count = 2;
            //Declare a Video Filter, and IExtensionProvider::createVideoFilter will be called
            ExtensionMetaInfo i;
            i.type = EXTENSION_TYPE::VIDEO_PRE_PROCESSING_FILTER;
            i.extension_name = agora::extension::VIDEO_FILTER_NAME;
            extension_list[0] = i;

            //Declare an Audio Filter, and IExtensionProvider::createAudioFilter will be called
            ExtensionMetaInfo j;
            j.type = EXTENSION_TYPE::AUDIO_FILTER;
            j.extension_name = agora::extension::AUDIO_FILTER_NAME;
            extension_list[1] = j;
        }

        // Create a video plug-in. After the SDK calls this method, you need to return the IExtensionVideoFilter instance
        agora_refptr<agora::rtc::IExtensionVideoFilter> ExtensionProvider::createVideoFilter(const char* name) {
            auto videoFilter = new agora::RefCountedObject<agora::extension::ExtensionVideoFilter>(YUVProcessor_);
            return videoFilter;
        }

        // Create a video plug-in. After the SDK calls this method, you need to return the IAudioFilter instance
        agora_refptr<agora::rtc::IAudioFilter> ExtensionProvider::createAudioFilter(const char* name) {
            auto audioFilter = new agora::RefCountedObject<agora::extension::ExtensionAudioFilter>(name, audioProcessor_);
            return audioFilter;
        }

        agora_refptr<agora::rtc::IExtensionVideoSink> ExtensionProvider::createVideoSink(const char* name) {
            return nullptr;
        }

        void ExtensionProvider::setExtensionControl(rtc::IExtensionControl* control){
        }
    }
}
