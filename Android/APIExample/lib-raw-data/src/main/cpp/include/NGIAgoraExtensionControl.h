//
// Copyright (c) 2020 Agora.io. All rights reserved

// This program is confidential and proprietary to Agora.io.
// And may not be copied, reproduced, modified, disclosed to others, published
// or used, in whole or in part, without the express prior written permission
// of Agora.io.

#pragma once  // NOLINT(build/header_guard)
#include "AgoraBase.h"
#include "AgoraRefPtr.h"
#include "AgoraRefCountedObject.h"
#include "IAgoraLog.h"
#include "NGIAgoraVideoFrame.h"
#include "NGIAgoraExtensionProvider.h"

namespace agora {
namespace rtc {
class IExtensionProvider;
/**
 * Interface for handling agora extensions.
 */
class IExtensionControl {
 public:
  /**
   * Agora Extension Capabilities.
   */
  struct Capabilities {
    /**
     * Whether to support audio extensions.
     */
    bool audio;
    /**
     * Whether to support video extensions.
     */
    bool video;
  };

  /**
   * Gets the capabilities of agora extensions.
   *
   * @param capabilities Supported extension capabilities.
   */
  virtual void getCapabilities(Capabilities& capabilities) = 0;

  /**
   * Recycles internal frame memory with a specified Video frame type.
   *
   * The SDK automatically recycles deprecated video frames. However,
   * you can still call this method to perform an immediate memory recycle.
   * @param type Frame type to be recycled.
   */
  virtual void recycleVideoCache() = 0;

  /**
   * This method dumps the content of the video frame to the specified file.
   *
   * @return
   * - 0: The method call succeeds.
   * - <0: The method call fails.
   */
  virtual int dumpVideoFrame(agora_refptr<IVideoFrame> frame, const char* file) = 0;

  /**
   * Sets log file.
   *
   * @param level Logging level. See #commons::LOG_LEVEL.
   * @param message Message to add to the log file.
   * @return
   * - 0: The method call succeeds.
   * - <0: The method call fails.
   */
  virtual int log(commons::LOG_LEVEL level, const char* message) = 0;

  /**
   * Post extension events to SDK.
   *
   * @param provider name of the provider
   * @param extension name of the extension
   * @param event_key key of the extension event
   * @param event_json_str string of the extension event
   * @return
   * - 0: The method call succeeds.
   * - <0: The method call fails.
   */
  virtual int fireEvent(const char* provider, const char* extension, const char* event_key, const char* value) = 0;

  /**
   * Register provider to the SDK
   * @param provider name of the provider
   * @param instance instance of the provider
   */
  virtual int registerProvider(const char* provider, agora_refptr<IExtensionProvider> instance) = 0;

 protected:
  virtual ~IExtensionControl() {}
};

}  // namespace rtc
}  // namespace agora
