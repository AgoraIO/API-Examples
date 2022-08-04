//
//  Agora SDK
//
//  Copyright (c) 2019 Agora.io. All rights reserved.
//

#pragma once  // NOLINT(build/header_guard)

#include "AgoraBase.h"
#include "AgoraRefPtr.h"

namespace agora {
namespace rtc {

/**
 * The IScreenCapturer class, which provides access to the screen capturer.
 */
class IScreenCapturer : public RefCountInterface {
 public:
#if defined (_WIN32) || (defined(__APPLE__) && TARGET_OS_MAC && !TARGET_OS_IPHONE)
  /**
   * Initializes the screen capturer by specifying a display ID.
   *
   * @note
   * This method applies to macOS only.
   *
   * This method shares a whole or part of a screen specified by the display ID.
   * @param displayId The display ID of the screen to be shared. This parameter specifies which screen you want
   * to share.
   * @param regionRect The reference to the relative location of the region to the screen: Rectangle.
   * - If the specified region overruns the screen, only the region within the screen will be captured.
   * - If you set `width` or `height` as 0, the whole screen will be captured.
   * Note that the coordinates of rectangle are relative to the window and follows system specifications.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int initWithDisplayId(uint32_t displayId, const Rectangle& regionRect) = 0;
#endif
 
#if defined(_WIN32) || (defined(__linux__) && !defined(__ANDROID__))
  /**
   * Initializes the screen capturer by specifying a screen Rect.
   *
   * @note
   * This method applies to Windows only.
   *
   * This method shares a whole or part of a screen specified by the screen Rect.
   * @param screenRect The reference to the Rect of the screen to be shared. This parameter specifies which screen you want
   * to share.
   * @param regionRect The reference to the relative location of the region to the screen. See \ref agora::rtc::Rectangle &regionRect "regionRect".
   * - If the specified region overruns the screen, only the region within the screen will be captured.
   * - If you set `width` or `height` as 0, the whole screen will be captured.
   * Note that the coordinates of rectangle are relative to the window and follows system specifications.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int initWithScreenRect(const Rectangle& screenRect,
                                 const Rectangle& regionRect) = 0;
#endif  // TARGET_OS_MAC && !TARGET_OS_IPHONE

  /**
   * Initializes the screen capturer by specifying a window ID.
   *
   * This method shares a whole or part of a window specified by the window ID.
   *
   * @note
   * This method applies to Windows and macOS only.
   * @param windowId The ID of the window to be shared. This parameter specifies which window you want
   * to share.
   * @param regionRect The reference to the relative location of the region to the window. See \ref agora::rtc::Rectangle &regionRect "regionRect".
   * - If the specified region overruns the window, only the region within the screen will be captured.
   * - If you set `width` or `height` as 0, the whole window will be captured.
   * Note that the coordinates of rectangle are relative to the window and follows system specifications.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int initWithWindowId(view_t windowId, const Rectangle& regionRect) = 0;

  /**
   * Sets the content hint for screen sharing.
   *
   * A content hint suggests the type of the content being shared, so that the SDK applies different
   * optimization algorithms to different types of content.
   * @param contentHint The content hint for screen capture: \ref rtc::VIDEO_CONTENT_HINT "VIDEO_CONTENT_HINT".
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - ERR_NOT_READY: No screen or window is being shared.
   */
  virtual int setContentHint(VIDEO_CONTENT_HINT contentHint) = 0;

  /**
   * Updates the screen capture region.
   * @param regionRect The reference to the relative location of the region to the screen or window. See \ref agora::rtc::Rectangle "Rectangle".
   * - If the specified region overruns the screen or window, the screen capturer captures only the region within it.
   * - If you set `width` or `height` as 0, the SDK shares the whole screen or window.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - No screen or window is being shared.
   */
  virtual int updateScreenCaptureRegion(const Rectangle& regionRect) = 0;

  /**
   * Set orientation of the captured screen image
   * @param VIDEO_ORIENTATION orientaion of the device 0(by default), 90, 180, 270
   */
  virtual void setScreenOrientation(VIDEO_ORIENTATION orientation) = 0;

  /**
   * Set frame rate of the screen capture source
   * @param rate frame rate (in fps)
   */
  virtual void setFrameRate(int rate) = 0;

#if defined(__ANDROID__)
  /**
   * Initializes the screen capturer by specifying the Intent data obtained from MediaProjection.
   *
   * This method shares the whole screen.
   *
   * @note
   * This method applies to Android only.
   * @param data The Intent data from `onActivityResult (int requestCode, int resultCode, Intent data)` after you create
   * an Intent from `MediaProjection` and sends the Intent to `startActivityForResult`.
   * @param dimensions The reference to the captured screen's resolution in terms of width &times; height.
   * - If you set `width` or `height` as 0, the dimensions will be the screen's width &times; height.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - ERR_INVALID_ARGUMENT if data is null.
   */
  virtual int initWithMediaProjectionPermissionResultData(void* data,
                                                          const VideoDimensions& dimensions) = 0;
#endif  // __ANDROID__

 protected:
  ~IScreenCapturer() {}
};

#if defined(__ANDROID__) || TARGET_OS_IPHONE
class IScreenCapturer2 : public RefCountInterface {
 public:
  /**
   * Screen capture's resolution.
   *
   * @param dimensions The reference to the captured screen's resolution in terms of width &times; height.
   * - If you set `width` or `height` as 0, the dimensions will be the screen's width &times; height.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - ERR_INVALID_ARGUMENT if data is null.
   */
  virtual int setScreenCaptureDimensions(const VideoDimensions& dimensions) = 0;

  /**
   * Updates the screen capture region.
   * @param regionRect The reference to the relative location of the region to the screen or window. See \ref agora::rtc::Rectangle "Rectangle".
   * - If the specified region overruns the screen or window, the screen capturer captures only the region within it.
   * - If you set `width` or `height` as 0, the SDK shares the whole screen or window.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   *   - No screen or window is being shared.
   */
  virtual int updateScreenCaptureRegion(const Rectangle& regionRect) = 0;

  /**
   * Set frame rate of the screen capture source
   * @param rate frame rate (in fps)
   */
  virtual int setFrameRate(int rate) = 0;

  /**
   * Set channels and sample rate of screen audio capturing
   * @param channels: channels of system audio capture
   * @param sampleRate: sample rate of system audio capture
   * @return
   * - 0: Sucess.
   * - < 0: Failure
   */
  virtual int setAudioRecordConfig(int channels, int sampleRate) = 0;

  /**
   * Set volume of screen audio capturing
   * @param volume: volume of system audio capture
   * @return
   * - 0: Sucess.
   * - < 0: Failure
   */
  virtual int setAudioVolume(uint32_t volume) = 0;

 protected:
  virtual ~IScreenCapturer2() {}
};
#endif

}  // namespace rtc
}  // namespace agora
