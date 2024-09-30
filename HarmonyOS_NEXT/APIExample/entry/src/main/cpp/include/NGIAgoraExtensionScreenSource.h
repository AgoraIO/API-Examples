//
//  Agora SDK
//
//  Copyright (c) 2021 Agora.io. All rights reserved.
//

#pragma once  // NOLINT(build/header_guard)

#include "AgoraBase.h"
#include "IAgoraLog.h"
#include "AgoraRefPtr.h"
#include "NGIAgoraVideoFrame.h"
#include "AgoraMediaBase.h"
#if defined(WEBRTC_IOS)
#include <coreVideo/CVPixelBuffer.h>
#endif
namespace agora {
namespace rtc {

struct ScreenCaptureProfilingStatistics {
  int capture_type;
  uint32_t captured_frame_width;
  uint32_t captured_frame_height;
  uint32_t total_captured_frames;
  uint64_t per_frame_cap_time_ms;
  uint64_t per_capture_cpu_cycles;
  bool capture_mouse_cursor;
  uint32_t target_fps;
  uint32_t real_capture_fps;
  uint32_t capture_out_fps;
  uint32_t drop_fps;

  ScreenCaptureProfilingStatistics()
    : capture_type(-1), captured_frame_width(0), captured_frame_height(0),
      total_captured_frames(0), per_frame_cap_time_ms(0),
      per_capture_cpu_cycles(0), capture_mouse_cursor(true),
      target_fps(0), real_capture_fps(0),
      capture_out_fps(0), drop_fps(0) {}
};

class IScreenCaptureSource : public RefCountInterface {
 public:
  class Control : public RefCountInterface {
  public:
    virtual int postEvent(const char* key, const char* value) = 0;
    virtual void printLog(commons::LOG_LEVEL level, const char* format, ...) = 0;
    virtual void printLog(commons::LOG_LEVEL level, const char* fromat, va_list) = 0;
    virtual int pushAudioFrame(const media::IAudioFrameObserver::AudioFrame& captured_frame) = 0;
    virtual bool timeToPushVideo() = 0;
    virtual int pushVideoFrame(const agora::agora_refptr<IVideoFrame>& captured_frame) = 0;
    virtual agora::agora_refptr<IVideoFrameMemoryPool> getMemoryPool() = 0;
#if defined(WEBRTC_IOS)
    virtual int pushVideoFrame(CVPixelBufferRef pixelBuffer, int64_t timestamp_ms, uint32_t rotation) = 0;
#endif
#if defined(WEBRTC_WIN)
    virtual void recycleVideoCache() = 0;
#endif
  };

  struct AudioCaptureConfig {
    uint32_t volume;
    int sample_rate_hz;
    int num_channels;
    AudioCaptureConfig() : volume(0), sample_rate_hz(0), num_channels(0) {}
  };

#if defined (__ANDROID__) || (defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE) || defined(__OHOS__)
  struct VideoCaptureConfig {
    agora::rtc::VideoDimensions dimensions;
    int frameRate;
    VideoCaptureConfig()
      : dimensions(640, 360), frameRate(15) {}
  };
#else
  struct VideoCaptureConfig {
    enum CaptureType {
      CaptureWindow,
      CaptureScreen,
    };
    CaptureType type;
    Rectangle screen_rect;
    Rectangle region_offset;
    uint32_t display_id;
    view_t window_id;

    // Mac only
    bool disable_avf;

    VideoCaptureConfig()
      : type(CaptureScreen), screen_rect(), region_offset(), display_id(0), window_id(NULL), disable_avf(false) { }
  };
#endif

  struct ScreenSourceListOption {
    int thumb_width;
    int thumb_height;
    int icon_width;
    int icon_height;
    bool include_screen;
    bool unfiltered;
    bool unskip_system_window;
  };
  
  enum CaptureMode {
    kPull, // SDK needs to poll the captured frame actively
    kPush // Capture source pushes the captured frame to sdk
  };


  virtual ~IScreenCaptureSource() {}

  virtual int initializeCapture(const agora_refptr<Control>& control) = 0;

  virtual int initVideo(const VideoCaptureConfig& config) = 0;
  // Start video capture interface for desktop capturing
  virtual int startVideoCapture(const VideoCaptureConfig& config) = 0;
  virtual int stopVideoCapture() = 0;
  virtual int stopVideoCaptureAsync() = 0;

  virtual CaptureMode getVideoCaptureMode() = 0;

  // Implementation of the following interfaces are not mandatory
  virtual int startAudioCapture(const AudioCaptureConfig& config) {
    return -ERR_NOT_SUPPORTED;
  }
  virtual int stopAudioCapture() {
    return -ERR_NOT_SUPPORTED;
  }
  virtual int setAudioVolume(uint32_t volume) {
    return -ERR_NOT_SUPPORTED;
  }
  virtual int setFrameRate(int fps) {
    return -ERR_NOT_SUPPORTED;
  }

  virtual int setScreenCaptureDimensions(const agora::rtc::VideoDimensions& dimensions) {
    return -ERR_NOT_SUPPORTED;
  }
  virtual int updateCaptureRegion(const agora::rtc::Rectangle& captureRegion) {
    return -ERR_NOT_SUPPORTED;
  }
  virtual int setExcludeWindowList(void* const * handles, int count) {
    return -ERR_NOT_SUPPORTED;
  }
  virtual int captureMouseCursor(bool capture) {
    return -ERR_NOT_SUPPORTED;
  }
  virtual int capture(agora::agora_refptr<IVideoFrame>& frame) {
    return -ERR_NOT_SUPPORTED;
  }
  virtual int getProfilingStats(ScreenCaptureProfilingStatistics& stats) {
    return -ERR_NOT_SUPPORTED;
  }
  virtual int getScreenDimensions(int& width, int& height) {
    return ERR_NOT_SUPPORTED;
  }
  virtual int setProperty(const char* key, const char* json_value) {
    return -ERR_NOT_SUPPORTED;
  }
  virtual int setCustomContext(const char* key, const void* context) {
    return -ERR_NOT_SUPPORTED;
  }
  virtual int getProperty(const char* key, char* json_value, int& length) {
    return -ERR_NOT_SUPPORTED;
  }
  virtual int setHighLight(bool isHighLight, unsigned int color, int width) {
    return -ERR_NOT_SUPPORTED;
  }
  virtual int setContentType(int contentType) {
    return -ERR_NOT_SUPPORTED;
  }
  virtual void* getCustomContext(const char* key) {
    return NULL;
  }
  virtual void* getScreenCaptureSources(const ScreenSourceListOption& option) {
    return NULL;
  }
  virtual void setFrameCopy(bool frameCopy) {};
#if defined(WEBRTC_ANDROID) && !defined(RTC_EXCLUDE_JAVA)
  virtual void setSmoothCapture(bool smoothCapture) {};
  virtual void setResetVitualDisplay(bool reset) {};
#endif
};

}  // namespace rtc
}  // namespace agora
