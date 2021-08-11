//
// Copyright (c) 2020 Agora.io. All rights reserved

// This program is confidential and proprietary to Agora.io.
// And may not be copied, reproduced, modified, disclosed to others, published
// or used, in whole or in part, without the express prior written permission
// of Agora.io.

#pragma once  // NOLINT(build/header_guard)
#include "AgoraBase.h"
#include "AgoraRefPtr.h"
#include "IAgoraLog.h"
#include "NGIAgoraVideoFrame.h"

namespace agora {
namespace rtc {
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
   * This method creates an IVideoFrame object with specified type, format, width and height
   * @return
   * - The pointer to {@link IVideoFrame}: The method call succeeds.
   * - An empty pointer nullptr: The method call fails.
   */
  virtual agora_refptr<IVideoFrame> createVideoFrame(
      IVideoFrame::Type type, IVideoFrame::Format format, int width, int height) = 0;

  /**
   * Creates a new IVideoFrame object by copying from the source video frame.
   *
   * @return
   * - The pointer to {@link IVideoFrame}: The method call succeeds.
   * - An empty pointer nullptr: The method call fails.
   */
  virtual agora_refptr<IVideoFrame> copyVideoFrame(agora_refptr<IVideoFrame> src) = 0;

  /**
   * Recycles internal frame memory with a specified Video frame type.
   *
   * The SDK automatically recycles deprecated video frames. However,
   * you can still call this method to perform an immediate memory recycle.
   * @param type Frame type to be recycled.
   */
  virtual void recycleVideoCache(IVideoFrame::Type type) = 0;

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

  virtual int fireEvent(const char* id, const char* event_key, const char* event_json_str) = 0;

 protected:
  virtual ~IExtensionControl() {}
};

}  // namespace rtc
}  // namespace agora
