#pragma once
#include "AgoraRefPtr.h"
#include "AgoraBase.h"
#include "IAgoraLog.h"
#include "NGIAgoraVideoFrame.h"
#include "AgoraExtensionVersion.h"

#ifndef OPTIONAL_PROCESSRESULT_SPECIFIER
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1800)
#define OPTIONAL_PROCESSRESULT_SPECIFIER ProcessResult::
#else
#define OPTIONAL_PROCESSRESULT_SPECIFIER
#endif
#endif

namespace webrtc {
struct AudioFrameHandleInfo;
}  // namespace webrtc

namespace agora {
namespace rtc {
/**
 * The `IAudioFilterBase` class is the base class for audio filters. You can use this class to implement your own filter
 * and add it to an audio track.
 */
class IAudioFilterBase : public RefCountInterface {
 public:
  // Return false if filter decide to drop the frame.
  // Implementation should not change samples_per_channel_/sample_rate_hz_/num_channels_
  // Implementation should just adapt data of audio frame.
  /**
   * Adapts the audio frame.
   * @param inAudioFrame The reference to the audio frame that you want to adapt.
   * @param adaptedFrame The reference to the adapted audio frame.
   * @return
   * - `true`: Success.
   * - `false`: Failure. For example, the IAudiofilter object drops the audio frame.
   */
  virtual bool adaptAudioFrame(const media::base::AudioPcmFrame& inAudioFrame,
                               media::base::AudioPcmFrame& adaptedFrame) = 0;

  /**
   * Get the sample rate supported by the audio filter, the framework will resample
   * the audio data and then pass it to the audio filter.
   * @return
   * - 0: Audio data will not be resampled.
   * - > 0: Audio data will be resampled to this sample rate.
   */
  virtual int getPreferredSampleRate() = 0;

  /**
   * Get the channel number supported by the audio filter, the framework will resample
   * the audio data and then pass it to the audio filter. If the user does not
   * overwrite, resampling will not be done by default.
   * @return
   * - 0: Audio data will not be resampled.
   * - > 0: Audio data will be resampled to this sample rate.
   */
  virtual int getPreferredChannelNumbers() = 0;

 protected:
  ~IAudioFilterBase() {}
};

/**
 * The `IAudioFilter` class.
 *
 * This class is the intermediate node for audio, which reads audio frames from the underlying
 * pipeline and writes audio frames back after adaptation.
 */
class IAudioFilter : public IAudioFilterBase {
 public:
  /**
   * Enables or disables the audio filter.
   * @param enable Whether to enable the audio filter:
   * - `true`: Enable the audio filter.
   * - `false`: Do not enable the audio filter.
   */
  virtual void setEnabled(bool enable) = 0;
  /**
   * Checks whether the audio filter is enabled.
   * @return
   * - `true`: The audio filter is enabled.
   * - `false`: The audio filter is not enabled.
   */
  virtual bool isEnabled() const = 0;
  /**
   * Sets a private property in the `IAudioFilter` class.
   *
   * @param key The pointer to the property name.
   * @param buf The pointer to the buffer of this private property.
   * @param buf_size The buffer size of this private property.
   * @return
   * - The actual size of the private property, if the method call succeeds.
   * - -1, if the method call fails.
   */
  virtual int setProperty(const char* key, const void* buf, int buf_size) = 0;
  /**
   * Gets a private property in the `IAudioFilter` class.
   *
   * @param name The pointer to the property name.
   * @param buf The pointer to the buffer of this private property.
   * @param buf_size The buffer size of this private property.
   * @return
   * - The actual size of the private property, if the method call succeeds.
   * - -1, if the method call fails.
   */
  virtual int getProperty(const char* key, void* buf, int buf_size) const = 0;
  /**
   * Gets the name of the `IAudioFilter` class.
   *
   * @return
   * - The name of the audio filter, if the method call succeeds.
   * - An empty string, if the method call fails.
   */
  virtual const char * getName() const = 0;

  /**
   * Get the sample rate supported by the audio filter, the framework will resample
   * the audio data and then pass it to the audio filter. If the user does not
   * overwrite, resampling will not be done by default.
   * @return
   * - 0: Audio data will not be resampled.
   * - > 0: Audio data will be resampled to this sample rate.
   */
  virtual int getPreferredSampleRate() { return 0; };

  /**
   * Get the channel number supported by the audio filter, the framework will resample
   * the audio data and then pass it to the audio filter. If the user does not
   * overwrite, resampling will not be done by default.
   * @return
   * - 0: Audio data will not be resampled.
   * - > 0: Audio data will be resampled to this sample rate.
   */
  virtual int getPreferredChannelNumbers() { return 0; };

 protected:
  ~IAudioFilter() {}
};

/**
 * The `IVideoFilterBase` class is the base class for video filters. You can use this class to implement your own filter
 * and add the filter to a video track.
 */
class IVideoFilterBase : public RefCountInterface {
 public:
  /**
   * Adapts the video frame.
   *
   * @param capturedFrame The reference to the captured video frame that you want to adapt.
   * @param adaptedFrame The reference to the adapted video frame.
   *
   * @return
   * - `true`: Success.
   * - `false`: Failure, if, for example, the `IVideofilter` object drops the video frame.
   */
  virtual bool adaptVideoFrame(const media::base::VideoFrame& capturedFrame,
                               media::base::VideoFrame& adaptedFrame) = 0;
};

// TODO(Bob): private inherit?
/**
 * The IVideoFilter class.
 *
 * This class is the intermediate node for video, which contains both the video source and the video
 * sink. It reads video frames from the underlying video pipeline and writes video frames back after
 * adaptation.
 */
class IVideoFilter : public IVideoFilterBase {
 public:
  /**
   * Enables or disables the video filter.
   * @param enable Whether to enable the video filter:
   * - `true`: (Default) Enable the video filter.
   * - `false`: Do not enable the video filter. If the filter is disabled, frames will be passed without
   * adaption.
   */
  virtual void setEnabled(bool enable) {}
  /**
   * Checks whether the video filter is enabled.
   * @return
   * - `true`: The video filter is enabled.
   * - `false`: The video filter is not enabled.
   */
  virtual bool isEnabled() { return true; }
  /**
   * Sets a private property in the `IVideoFilter` class.
   *
   * @param key The pointer to the property name.
   * @param buf The pointer to the buffer of this private property.
   * @param buf_size The buffer size of this private property.
   * @return
   * - The actual size of the private property, if the method call succeeds.
   * - -1, if the method call fails.
   */
  virtual int setProperty(const char* key, const void* buf, size_t buf_size) { return -1; }
  /**
   * Gets a private property in the IVideoFilter class.
   *
   * @param key The pointer to the property name.
   * @param buf The pointer to the buffer of this private property.
   * @param buf_size The buffer size of this private property.
   * @return
   * - The actual size of the private property, if the method call succeeds.
   * - -1, if the method call fails.
   */
  virtual int getProperty(const char* key, void* buf, size_t buf_size) { return -1; }
  /**
   * This function is invoked right before data stream starts.
   * Custom filter can override this function for initialization.
   * @return
   * - `true`: The initialization succeeds.
   * - `false`: The initialization fails.
   */
  virtual bool onDataStreamWillStart() { return true; }
   /**
   * This function is invoked right before data stream stops.
   * Custom filter can override this function for deinitialization.
   */
  virtual void onDataStreamWillStop() { }
  /**
   * This function indicates if the filter is for internal use.
   * @note Do not override this function.
   * @return
   * - `true`: The filter is implemented by external users.
   * - `false`: The filter is implemented by internal users.
   */
  virtual bool isExternal() { return true; }
  /**
   * This function indicates if the filter is implemented by third-party providers.
   * @note Do not override this function.
   * @return
   * - `true`: The filter is implemented by third-party providers.
   * - `false`: otherwise.
   */
  virtual bool isExtensionFilter() { return false; }
};

/**
 * The IExtensionVideoFilter class.
 *
 * This class defines the interfaces that a external video extension provider can implement
 * so as to be loaded by SDK as an "3rd party extension" for video pre- or post- processing.
 */
class IExtensionVideoFilter : public IVideoFilter {
 public:
  enum ProcessMode {
    kSync, // Indicates that video frame data will be exchanged via "adaptVideoFrame"
    kAsync, // Indicates that video frame data will be exchanged via "pendVideoFrame" & "deliverVideoFrame"
  };

  enum ProcessResult {
    kSuccess, // Video frame data is successfully processed
    kBypass,  // Video frame data should bypass the current filter and flow to its successsors
    kDrop, // Video Frame data should be discarded
  };

  /**
   * The IExtensionVideoFilter::Control class.
   *
   * This class defines the interfaces that the extension filter can leverage to interact with the SDK.
   * The "IExtensionVideoFilter::Control" object will be passed to the filter when SDK invoke the filter's
   * "start" interface.
   */
  class Control : public RefCountInterface {
   public:
    /**
     * @brief Filter can invoke this function to deliver the processed frame to SDK if the Process Mode is
     * designated as "kAsync" by the filter via "getProcessMode".
     * @param frame the processed video frame
     * @return see @ref ProcessResult
     */
    virtual ProcessResult deliverVideoFrame(agora::agora_refptr<IVideoFrame> frame) = 0;
    /**
     * @brief Filter can invoke this function to get the IVideoFrameMemoryPool object if a new IVideoFrame
     *  data object is needed.
     */
    virtual agora::agora_refptr<IVideoFrameMemoryPool> getMemoryPool() = 0;
    /**
     * @brief Post an event and notify the end users.
     * @param key '\0' ended string that describes the key of the event
     * @param value '\0' ended string that describes the value of the event
     */
    virtual int postEvent(const char* key, const char* value) = 0;
    /**
     * @brief print log to the SDK.
     * @param level Log level @ref agora::commons::LOG_LEVEL
     * @param format log formatter string
     * @param ... variadic arguments
     */
    virtual void printLog(commons::LOG_LEVEL level, const char* format, ...) = 0;
    /**
     * @brief Ask SDK to disable the current filter if a fatal error is detected
     * @param error error code
     * @param msg error message
     */
    virtual void disableMe(int error, const char* msg) = 0;
  };

  /**
   * @brief SDK will invoke this API first to get the filter's requested process mode @ref ProcessMode and threading model
   * @param mode [out] filter assign its desired the process mode @ref ProcessMode
   * @param independent_thread [out] filter assign its desired threading model. When this boolean is set "true", an
   * indepent thread will be assigned to the current filter and all invocations from SDK afterwards are ensured to
   * happen on that fixed thread. If this boolean flag is set "false", the filter will re-use the  thread of the SDK's
   * data path. All invocations from SDK afterwards are also ensured to be on the same thread, however that thread is shared.
   * @note If the filter implementation is not thread sensitive, we recommend to set the boolean to "false" to reduce thread context
   * switching.
   */
  virtual void getProcessMode(ProcessMode& mode, bool& independent_thread) = 0;
  /**
   * @brief SDK will invoke this API before feeding video frame data to the filter. Filter can perform its initialization/preparation job
   * in this step.
   *
   * @param control object to @ref IExtensionFilter::Control that pass to the filter which can be used for future interaction with the SDK
   * @return error code
   */
  virtual int start(agora::agora_refptr<Control> control) = 0;
  /**
   * @brief SDK will invoke this API when the data stream is about to stop. Filter can perform cleanup jobs in this step
   *
   * @return error code
   */
  virtual int stop() = 0;
  /**
   * @brief SDK will invoke this API every time before sending data to the filter. Filter can desigante the type @ref VideoFrameInfo::Type
   * and format @ref MemPixelBuffer::Format of the next frame. SDK will then try to perform type / format conversion before sending data to
   * the filter.
   *
   * @param type requested type of the next frame
   * @param format requested formant of the next frame
   */
  virtual void getVideoFormatWanted(VideoFrameData::Type& type, RawPixelBuffer::Format& format) = 0;
  /**
   * @brief SDK will invoke this API to send video frame to the filter if process mode is "Async". Filter invokes control's "deliverFrame"
   * to send back the frame after processing.
   *
   * @param frame frame pending for processing
   */
  virtual ProcessResult pendVideoFrame(agora::agora_refptr<IVideoFrame> frame) {
    return OPTIONAL_PROCESSRESULT_SPECIFIER kBypass;
  }
  /**
   * @brief SDK will invoke this API to send video frame to the filter if process mode is "Sync".
   *
   * @param frame frame pending for processing
   */
  virtual ProcessResult adaptVideoFrame(agora::agora_refptr<IVideoFrame> in, agora::agora_refptr<IVideoFrame>& out) {
    return OPTIONAL_PROCESSRESULT_SPECIFIER kBypass;
  }
  /* Occurs each time needs to get rotation apply.
   *
   * @return Determines whether to rotate.
   * - true: need to rotate.
   * - false: no rotate.
   */
  virtual bool getRotationApplied() { return false; }

  // NOTE: The following two interfaces should never be overriden!
  virtual bool isExtensionFilter() { return true; }
  virtual bool adaptVideoFrame(
      const media::base::VideoFrame& capturedFrame,
      media::base::VideoFrame& adaptedFrame) {
    return -ERR_NOT_SUPPORTED;
  }
};

/**
 * The `IVideoSinkBase` class is the base class for the custom video sink.
 */
class IVideoSinkBase : public RefCountInterface {
 public:
  /**
   * Sets a private property in the `IVideoFilter` class.
   *
   * @param key The pointer to the property name.
   * @param buf The pointer to the buffer of this private property.
   * @param buf_size The buffer size of this private property.
   * @return
   * - The actual size of the private property, if the method call succeeds.
   * - -1, if the method call fails.
   */
  virtual int setProperty(const char* key, const void* buf, int buf_size) { return -1; }
  /**
   * Gets a private property in the `IVideoFilter` class.
   *
   * @param key The pointer to the property name.
   * @param buf The pointer to the buffer of this private property.
   * @param buf_size The buffer size of this private property.
   * @return
   * - The actual size of the private property, if the method call succeeds.
   * - -1, if the method call fails.
   */
  virtual int getProperty(const char* key, void* buf, int buf_size) { return -1; }
  /**
   * Occurs when the `IVideoSinkBase` object receives the video frame.
   * @param videoFrame The reference to the video frame.
   */
  virtual int onFrame(const media::base::VideoFrame& videoFrame) = 0;
  /**
   * Used internally to distinguish between external and internal sinks.
   * External application should not override this interface.
   */
  virtual bool isExternalSink() { return true; }
  /**
   * This function is invoked right before data stream starts.
   * Custom sink can override this function for initialization.
   * @return
   * - `true`, if initialization succeeds.
   * - `false`, if initialization fails.
   */
  virtual bool onDataStreamWillStart() { return true; }
  /**
   * This function is invoked right before data stream stops.
   * Custom sink can override this function for deinitialization.
   */
  virtual void onDataStreamWillStop() { }
  /**
   * Whether to mirror the video frame.
   * @return
   * - true: mirror the video frame.
   * - false: do not mirror the video frame.
   */
  virtual bool applyMirror() { return false; }
  /**
   * Whether to rotate the video frame.
   * @return
   * - true: rotate the video frame.
   * - false: do not rotate the video frame.
   */
  virtual bool applyRotation() { return false; }
};

class IMediaExtensionObserver : public RefCountInterface {
public:
  virtual ~IMediaExtensionObserver() {}
  virtual void onEvent(const char* provider, const char* extension, const char* key, const char* json_value) {}
  virtual void onExtensionStopped(const char* provider, const char* extension) {}
  virtual void onExtensionStarted(const char* provider, const char* extension) {}
  virtual void onExtensionError(const char* provider, const char* extension, int error, const char* message) {}
};

/**
 * The IAudioPcmDataSender class.
 *
 * In scenarios involving custom audio source, you can use the `IAudioPcmDataSender` class to send PCM audio data directly to the audio track. If the audio track is disabled, the sent audio data is automatically discarded.
 */
class IAudioPcmDataSender : public RefCountInterface {
 public:
  /**
   * Sends the PCM audio data to the local audio track.
   *
   * @param audio_data The PCM audio data to be sent.
   * @param samples_per_channel The number of audio samples in 10 ms for each audio channel.
   * @param bytes_per_sample The number of bytes in each sample.
   * @param number_of_channels The number of audio channels.
   * @param sample_rate The sample rate (Hz). The minimum value is 8000.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int sendAudioPcmData(
      const void* audio_data, uint32_t capture_timestamp,
      const size_t samples_per_channel,  // for 10ms Data, number_of_samples * 100 = sample_rate
      const agora::rtc::BYTES_PER_SAMPLE bytes_per_sample,     // 2
      const size_t number_of_channels,
      const uint32_t sample_rate) = 0; // sample_rate > 8000)

 protected:
  ~IAudioPcmDataSender() {}
};

/**
 * The `IAudioEncodedFrameSender` class.
 *
 * In scenarios involving custom audio source, you can use the `IAudioEncodedFrameSender` class to
 * send encoded audio data directly to the audio track. If the track is disabled, the sent audio
 * data will be automatically discarded.
 */
class IAudioEncodedFrameSender : public RefCountInterface {
 public:
  /**
   * Sends the encoded audio frame to the local audio track.
   *
   * @param payload_data The pointer to the payload data.
   * @param payload_size The payload size.
   * @param audioFrameInfo The reference to the information of the audio frame:
   * \ref agora::rtc::EncodedAudioFrameInfo "EncodedAudioFrameInfo".
   *
   * @return
   * - `true`: Success.
   * - `false`: Failure.
   */
  virtual bool sendEncodedAudioFrame(const uint8_t* payload_data, size_t payload_size,
                                     const EncodedAudioFrameInfo& audioFrameInfo) = 0;

 protected:
  ~IAudioEncodedFrameSender() {}
};

class IAudioEncodedFrameReceiver {
 public:
  /**
   * Occurs when the track receives an audio encodeed frame packet.
   *
   * @param packet The pointer to the audio packet.
   * @param length The length of the packet.
   * @param info The packet info.
   *
   */
  virtual bool onEncodedAudioFrameReceived (
      const uint8_t *packet, size_t length, const media::base::AudioEncodedFrameInfo& info) = 0;

  virtual ~IAudioEncodedFrameReceiver() {}
};

/**
 * The IMediaPacketReceiver class. You can register a receiver in remote audio or video tracks to trigger
 * callbacks when RTP/UDP packets are received.
 */
class IMediaPacketReceiver {
 public:
  /**
   * Occurs when the track receives a media packet.
   *
   * @param packet The pointer to the media packet.
   * @param length The length of the packet.
   * @param options The packet info.
   *
   */
  virtual bool onMediaPacketReceived(
      const uint8_t *packet, size_t length, const agora::media::base::PacketOptions& options) = 0;

  virtual ~IMediaPacketReceiver() {}
};

/**
 * The IMediaControlPacketReceiver class.
 *
 * You can register a receiver in audio or video tracks to trigger callbacks
 * when RTCP/UDP packets are received.
 */
class IMediaControlPacketReceiver {
 public:
  /**
   * Occurs when the track receives media control packet.
   *
   * @param packet The pointer to the media packet.
   * @param length The length of the packet.
   *
   */
  virtual bool onMediaControlPacketReceived(uid_t uid, const uint8_t *packet, size_t length) = 0;

  virtual ~IMediaControlPacketReceiver() {}
};

/**
 * The `IMediaPacketSender` class.
 *
 * You can use the `IMediaPacketSender` class to create a LocalVideoTrack or LocalAudioTrack,
 * and then send media packets directly to the track. The media packets are RTP/UDP packets that contain
 * media payload. If the track is disabled, the packets will be automatically discarded.
 */
class IMediaPacketSender : public RefCountInterface {
 public:
  /**
   * Sends the frame packet to the local track.
   *
   * @param packet The pointer to the packet.
   * @param length The packet size.
   * @param options The packet information: {@link media::base::PacketOptions PacketOptions}.
   *
   * @return
   * - `true`: Success.
   * - `false`: Failure.
   */
  virtual int sendMediaPacket(const uint8_t *packet, size_t length,
                              const media::base::PacketOptions &options) = 0;
 protected:
  ~IMediaPacketSender() {}
};

/**
 * The `IMediaControlPacketSender` class.
 *
 * You can get the `IMediaControlPacketSender` class object from a video track or audio track,
 * and then send media control packets directly. The media control packets are RTCP/UDP packets that contain
 * media control payload. If the track is disabled, the packets will be automatically discarded.
 */
class IMediaControlPacketSender {
 public:
  /**
   * Sends the media control packet to a specified user.
   * Currently, we only support sending packets through video tracks.
   *
   * @param userId ID of the user to send the packet to.
   * @param packet The pointer to the packet.
   * @param length The packet size.
   *
   * @return
   * - `true`: Success.
   * - `false`: Failure.
   */
  virtual int sendPeerMediaControlPacket(media::base::user_id_t userId,
                                         const uint8_t *packet,
                                         size_t length) = 0;

  /**
   * Sends the media transport control packet to all users.
   * Currently, we only support sending packets through video tracks.
   * @param packet The pointer to the packet.
   * @param length The packet size.
   *
   * @return
   * - `true`: Success.
   * - `false`: Failure.
   */
  virtual int sendBroadcastMediaControlPacket(const uint8_t *packet, size_t length) = 0;

  virtual ~IMediaControlPacketSender() {}
};

/**
 * The `IAudioSinkBase` class is the base class for the audio sink. You can use this class to implement your own sink
 * and add the sink to an audio track.
 */
class IAudioSinkBase : public RefCountInterface {
 public:
  /** Gets the audio frame.
   *
   * @param audioframe {@link media::base::AudioPcmFrame AudioPcmFrame}
   * @return
   * - `true`: Success.
   * - `false`: Failure.
   */
  virtual bool onAudioFrame(const media::base::AudioPcmFrame& audioFrame) = 0;

 protected:
  ~IAudioSinkBase() {}
};

/**
 * The `IVideoFrameSender` class.
 *
 * In scenarios involving custom video sources, you can use this class to send the video
 * data directly to a video track. If the video track is disabled, the sent data will
 * be automatically discarded.
 */
class IVideoFrameSender : public RefCountInterface {
 public:
  /**
   * Sends the video frame to the video track.
   *
   * @param videoFrame The reference to the video frame to send.
   *
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int sendVideoFrame(const media::base::ExternalVideoFrame& videoFrame) = 0;

 protected:
  ~IVideoFrameSender() {}
};

/**
 * The `IVideoEncodedImageSender` class.
 *
 * In scenarios involving custom video sources, you can use this class to send the encoded video data
 * directly to the video track. If the video track is disabled, the sent video image will be
 * automatically discarded.
 */
class IVideoEncodedImageSender : public RefCountInterface {
 public:
  /**
   * Sends the encoded video image to the video track.
   * @param imageBuffer The video buffer.
   * @param length The data length of the video data.
   * @param videoEncodedFrameInfo The reference to the information of the encoded video frame:
   * {@link EncodedVideoFrameInfo}.
   *
   * @return
   * - `true`: Success.
   * - `false`: Failure.
   */
  virtual bool sendEncodedVideoImage(const uint8_t* imageBuffer, size_t length,
                                     const EncodedVideoFrameInfo& videoEncodedFrameInfo) = 0;

 protected:
  ~IVideoEncodedImageSender() {}
};

/**
 * The IVideoBeautyFilter class.
 */
class IVideoBeautyFilter : public IVideoFilter {
 public:
  /**
   * The video image enhancement options.
   */
  struct BeautyOptions {
    /**
     * The lightening contrast level.
     */
    enum LIGHTENING_CONTRAST_LEVEL {
      /**
       * 0: Low contrast level.
       */
      LIGHTENING_CONTRAST_LOW = 0,
      /**
       * (Default) Normal contrast level.
       */
      LIGHTENING_CONTRAST_NORMAL,
      /**
       * High contrast level.
       */
      LIGHTENING_CONTRAST_HIGH
    };

    /**
     * The contrast level, usually used with {@link lighteningLevel} to brighten the video:
     * #LIGHTENING_CONTRAST_LEVEL.
     */
    LIGHTENING_CONTRAST_LEVEL lighteningContrastLevel;

    /**
     * The brightness level. The value ranges from 0.0 (original) to 1.0.
     */
    float lighteningLevel;

    /**
     * The sharpness level. The value ranges from 0.0 (original) to 1.0. This parameter is usually
     * used to remove blemishes.
     */
    float smoothnessLevel;

    /**
     * The redness level. The value ranges from 0.0 (original) to 1.0. This parameter adjusts the
     * red saturation level.
     */
    float rednessLevel;

    BeautyOptions(LIGHTENING_CONTRAST_LEVEL contrastLevel, float lightening, float smoothness,
                  float redness)
        : lighteningContrastLevel(contrastLevel),
          lighteningLevel(lightening),
          smoothnessLevel(smoothness),
          rednessLevel(redness) {}

    BeautyOptions()
        : lighteningContrastLevel(LIGHTENING_CONTRAST_NORMAL),
          lighteningLevel(0),
          smoothnessLevel(0),
          rednessLevel(0) {}
  };
  /**
   * Sets the image enhancement options.
   * @param enabled Whether to enable image enhancement.
   * - `true`: Enable image enhancement.
   * - `false`: Do not enable image enhancement.
   * @param options The image enhancement options: BeautyOptions.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setBeautyEffectOptions(bool enabled, BeautyOptions options) = 0;
};

/**
 * The `IVideoRenderer` class.
 */
class IVideoRenderer : public IVideoSinkBase {
 public:
  /**
   * Sets the render mode.
   * @param renderMode The video render mode.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setRenderMode(media::base::RENDER_MODE_TYPE renderMode) = 0;
  /**
   * Sets the render mode of the view.
   * @param view the view to set render mode.
   * @param renderMode The video render mode.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setRenderMode(void* view, media::base::RENDER_MODE_TYPE renderMode) = 0;
  /**
   * Sets whether to mirror the video.
   * @param mirror Whether to mirror the video:
   * - `true`: Mirror the video.
   * - `false`: Do not mirror the video.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setMirror(bool mirror) = 0;
  /**
   * Sets whether to mirror the video.
   * @param view the view to set mirror mode.
   * @param mirror Whether to mirror the video:
   * - `true`: Mirror the video.
   * - `false`: Do not mirror the video.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setMirror(void* view, bool mirror) = 0;
  /**
   * Sets the video display window.
   * @param view The pointer to the video display window.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int setView(void* view) = 0;
  /**
   * Sets the video display window.
   * @param view The pointer to the video display window.
   * @param cropArea (Optional) Sets the relative location of the region to show. See Rectangle.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int addView(void* view, const Rectangle& cropArea) = 0;
  /**
   * Stops rendering the video view on the window.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int unsetView() = 0;
  /**
   * remove rendering the video view on the window.
   * @return
   * - 0: Success.
   * - < 0: Failure.
   */
  virtual int removeView(void* view) = 0;
};

static const int kDeviceIdSize = 128;

class IVideoTrack;
/**
 * The `IVideoFrameTransceiver` class.
 */
class IVideoFrameTransceiver : public RefCountInterface {
 public:
  virtual int getTranscodingDelayMs() = 0;
  virtual int addVideoTrack(agora_refptr<IVideoTrack> track) = 0;
  virtual int removeVideoTrack(agora_refptr<IVideoTrack> track) = 0;
};

}
}
