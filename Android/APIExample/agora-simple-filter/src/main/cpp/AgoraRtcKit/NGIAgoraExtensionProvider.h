//
//  Agora SDK
//
//  Copyright (c) 2021 Agora.io. All rights reserved.
//

#pragma once

#include "AgoraRefPtr.h"
#include "NGIAgoraMediaNode.h"
#include "AgoraExtensionVersion.h"

namespace agora {
namespace rtc {
class IExtensionControl;

/**
 * Interfaces for Extension Provider
 * User can implement these interfaces for providing their own media node implementations to SDK.
 * Please refer to \ref agora::RefCountedObject to wrap your implementation so that it can be
 * held by agora::agora_refptr.
 * For example:
 * class YourExtensionProvide: public IExtensionProvider {
 *  // Your Concrete implementaion
 * };
 * 
 * // Use agroa::RefCountedObject to provide RefCountInterface implementation for you implementation,
 * // intantiate and wrap it with agora_refptr.
 * 
 * agora_refptr<IExtensionProvider> provider = new RefCountedObject<YourExtensionProvide>(Arg1, Arg2, ...);
 * 
 * You can instantiate your AudioFilter/VideoFilter/VideoSink in the same way.
 */

class IExtensionProvider : public RefCountInterface {
 public:
  enum EXTENSION_TYPE {
    AUDIO_FILTER,
    VIDEO_PRE_PROCESSING_FILTER,
    VIDEO_POST_PROCESSING_FILTER,
    AUDIO_SINK,
    VIDEO_SINK,
    UNKNOWN,
  };

  struct ExtensionMetaInfo {
    EXTENSION_TYPE type;
    const char* extension_name;
  };

  virtual void setExtensionControl(IExtensionControl* control) {}

  virtual void enumerateExtensions(ExtensionMetaInfo* extension_list,
                                   int& extension_count) {
    (void) extension_list;
    extension_count = 0;
  }

  virtual agora_refptr<IAudioFilter> createAudioFilter(const char* name) {
    return NULL;
  }

  virtual agora_refptr<IExtensionVideoFilter> createVideoFilter(const char* name) {
    return NULL;
  }

  virtual agora_refptr<IVideoSinkBase> createVideoSink(const char* name) {
    return NULL;
  }

  virtual void setProperty(const char* key, const char* value) {}

 protected:
  virtual ~IExtensionProvider() {}
};

class IExtensionProviderV2 : public IExtensionProvider {
 public:
  virtual void getExtensionVersion(const char* extension_name, ExtensionVersion& version) = 0;
};

}  // namespace rtc
}  // namespace agora
