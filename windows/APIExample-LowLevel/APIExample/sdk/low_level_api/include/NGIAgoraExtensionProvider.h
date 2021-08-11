#pragma once

#include "AgoraRefPtr.h"
#include "NGIAgoraMediaNode.h"

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
  enum PROVIDER_TYPE {
    LOCAL_AUDIO_FILTER,
    REMOTE_AUDIO_FILTER,
    LOCAL_VIDEO_FILTER,
    REMOTE_VIDEO_FILTER,
    LOCAL_VIDEO_SINK,
    REMOTE_VIDEO_SINK,
    UNKNOWN,
  };

  virtual PROVIDER_TYPE getProviderType() {
    return UNKNOWN;
  }

  virtual void setExtensionControl(IExtensionControl* control) {}

  virtual agora_refptr<IAudioFilter> createAudioFilter() {
    return NULL;
  }

  virtual agora_refptr<IVideoFilter> createVideoFilter() {
    return NULL;
  }

  virtual agora_refptr<IVideoSinkBase> createVideoSink() {
    return NULL;
  }

 protected:
  ~IExtensionProvider() {}
};

}
}
