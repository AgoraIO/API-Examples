#pragma once
#include "NGIAgoraMediaNode.h"

namespace agora {
namespace rtc {
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

class IAudioFilterV2 : public IAudioFilter {
public:
    class Control : public RefCountInterface {
     public:
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
    };
public:
    /**
       * @brief AgoraSDK set IAudioFilterV2::Control to filter
       * @param control IAudioFilterV2::Control
       */
    virtual void setExtensionControl(agora::agora_refptr<IAudioFilterV2::Control> control) = 0;
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
   * @brief SDK will invoke this API first to get the filter's requested process mode @ref ProcessMode
   * @param mode [out] filter assign its desired the process mode @ref ProcessMode
   * @param independent_thread deprecated. SDK will ignore this parameter.
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

class IExtensionVideoSink : public IVideoSinkBase {
};

class ILipSyncFilter : public RefCountInterface {
 public:
  enum ProcessResult {
    kSuccess, // Video frame data is successfully processed
    kBypass,  // Video frame data should bypass the current filter and flow to its successsors
    kDrop, // Video Frame data should be discarded
  };
  
  class Control : public RefCountInterface {
   public:
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
    /**
     * @brief report counter to the SDK.
     * @param counter_id counter id
     * @param value counter value
     */
    virtual void ReportCounter(int32_t counter_id, int32_t value) = 0;
    /**
     * @brief get stats to the SDK.
     * @param counter_id counter id
     */
    virtual int GetStats(int32_t counter_id) = 0;
  };
  
  virtual int start(agora::agora_refptr<Control> control) = 0;
  
  virtual int stop() = 0;
  
  virtual int setProperty(const char* key, const void* buf, size_t buf_size) { return -1; }
    /**
         * Convert the audio frame to face info.
         * @param inAudioFrame The reference to the audio frame that you want to convert.
       * @param outFaceInfo The reference to the face info.
       * @return see @ref ProcessResult
    */
  virtual ProcessResult convertAudioFrameToFaceInfo(const agora::media::base::AudioPcmFrame& inAudioFrame, char* outFaceInfo) {
    return kBypass;
  }
};

class ExtensionVideoCodecSettings {
 public:
  // Structure to hold settings for the video encoder.
  struct VideoEncoderSettings {
    uint32_t framerate = 0;              // The frame rate of the video in frames per second (fps).
    uint32_t bitrate_kbps = 0;           // The bitrate of the video in kilobits per second (kbps).
    uint32_t key_frame_interval = 0;     // The interval between key frames in seconds (time between I-frames).
  };
  // Structure to hold settings for the video decoder.
  struct VideoDecoderSettings {
    media::base::VIDEO_PIXEL_FORMAT prefer_data_format;   // The preferred pixel format for the decoded video.
  };
  
  uint16_t width = 0;                    // The width of the video frame in pixels.
  uint16_t height = 0;                   // The height of the video frame in pixels.
  VideoEncoderSettings encoder_settings; // An instance of VideoEncoderSettings to configure the encoder.
  VideoDecoderSettings decoder_settings; // An instance of VideoDecoderSettings to configure the decoder.
};

class ExtensionEncodedImage {
public:
  size_t length = 0;                                // The length of the encoded image data (in bytes).
  uint8_t* buffer = nullptr;                        // Pointer to the buffer holding the encoded image data (raw bytes).
  VIDEO_FRAME_TYPE frame_type = VIDEO_FRAME_TYPE_DELTA_FRAME;      // The type of the video frame (e.g., key frame, delta frame).
  uint32_t pts_ms = 0;                              // Presentation timestamp (PTS) in milliseconds, indicating when the frame should be displayed.
  uint32_t dts_ms = 0;                              // Decoding timestamp (DTS) in milliseconds, indicating when the frame should be decoded.
  uint32_t width = 0;                               // The width of the video frame in pixels.
  uint32_t height = 0;                              // The height of the video frame in pixels.
};

class IExtensionDecoderCallback {
 public:
  // Destructor for the IExtensionDecoderCallback class.
  virtual ~IExtensionDecoderCallback() {}
  /**
   * Called when a video frame has been successfully decoded.
   * @param frame A reference to the decoded video frame.
   * @param extended_info_json A JSON formatted string containing additional information about the decoded frame.
   */
  virtual void onDecodedFrame(agora_refptr<IVideoFrame> frame, const char* extended_info_json) = 0;
  /**
   * Logs messages at various log levels.
   * @param level The log level indicating the severity of the message (e.g., INFO, WARNING, ERROR).
   * @param format A format string for the log message, similar to printf formatting.
   * @param ... Additional arguments to format the log message.
   */
  virtual void printLog(commons::LOG_LEVEL level, const char* format, ...) = 0;
};

class IExtensionEncoderCallback {
 public:
  // Destructor for the IExtensionEncoderCallback class.
  virtual ~IExtensionEncoderCallback() {}
  /**
   * Called when a video frame has been successfully encoded.
   * @param img A pointer to the encoded image data.
   * @param extended_info_json A JSON formatted string containing additional information about the encoded frame.
   */
  virtual void onEncodedImage(const ExtensionEncodedImage* img, const char* extended_info_json) = 0;
  /**
   * Logs messages at various log levels.
   * @param level The log level indicating the severity of the message (e.g., INFO, WARNING, ERROR).
   * @param format A format string for the log message, similar to printf formatting.
   * @param ... Additional arguments to format the log message.
   */
  virtual void printLog(commons::LOG_LEVEL level, const char* format, ...) = 0;
};

class IExtensionVideoEncoder : public RefCountInterface  {
 public:
  /**
   * Initializes the video encoder with the specified codec settings.
   * @param codec_settings The video codec settings that define how the video should be encoded.
   * @return
   * - 0: Initialization succeeded.
   * - < 0: Initialization failed, possibly due to invalid settings or internal errors.
   */
  virtual int initVideoEncoder(ExtensionVideoCodecSettings codec_settings) = 0;
  /**
   * Registers a callback function to handle encoding events or notifications.
   * @param callback A pointer to the encoder callback interface.
   * @return
   * - 0: Registration succeeded.
   * - < 0: Registration failed, possibly due to an invalid callback pointer.
   */
  virtual int32_t registerEncoderCallback(IExtensionEncoderCallback* callback) = 0;
  /**
   * Encodes a video frame.
   * @param frame A reference to the video frame to be encoded.
   * @param frame_type The type of the video frame (e.g., keyframe, delta frame).
   * @return
   * - 0: Encoding succeeded.
   * - < 0: Encoding failed, possibly due to invalid frame data or internal errors.
   */
  virtual int encode(agora_refptr<IVideoFrame> frame, VIDEO_FRAME_TYPE frame_type) = 0;
  /**
   * Releases the resources used by the video encoder.
   * @return
   * - 0: Release succeeded.
   * - < 0: Release failed, possibly due to the encoder not being initialized or internal errors.
   */
  virtual int32_t releaseVideoEncoder() = 0;
  /**
   * Sets the rates for the encoder, specifying the target bitrate and framerate.
   * @param bitrate_kbps The target bitrate in kilobits per second.
   * @param framerate The target framerate in frames per second.
   * @return
   * - 0: Rate allocation succeeded.
   * - < 0: Rate allocation failed, possibly due to invalid parameters or internal errors.
   */
  virtual int32_t setRates(uint32_t bitrate_kbps, uint32_t framerate) = 0;
  /**
   * Queries the supported codec data formats.
   * @param data_format A pointer to an array where supported pixel formats will be filled.
   * @param size An input/output parameter; initially holds the size of the array, and will be updated with the actual number of formats filled.
   */
  virtual void querySupportCodecDataFormat(media::base::VIDEO_PIXEL_FORMAT* data_format, int& size) = 0;
  /**
   * Sets a advanced codec property in the `IExtensionVideoEncoder` class.
   *
   * @param key The pointer to the property name.
   * @param buf The pointer to the buffer of this private property.
   * @param buf_size The buffer size of this private property.
   * @return
   * - The actual size of the private property, if the method call succeeds.
   * - -1, if the method call fails.
   */
  virtual int setAdvancedCodecProperty(const char* key, const char* json_value) = 0;
  /**
   * Gets a advanced codec property in the `IExtensionVideoEncoder` class.
   *
   * @param name The pointer to the property name.
   * @param buf The pointer to the buffer of this advanced codec property.
   * @param buf_size The buffer size of this advanced codec property.
   * @return
   * - The actual size of the advanced codec property, if the method call succeeds.
   * - -1, if the method call fails.
   */
  virtual int getAdvancedCodecProperty(const char* key, char* json_value, int& length) const = 0;
};
 
class IExtensionVideoDecoder : public RefCountInterface  {
 public:
  /**
   * Initializes the video decoder with the specified codec settings.
   * @param codec_settings The video codec settings that specify how the video should be decoded.
   * @return
   * - 0: Initialization succeeded.
   * - < 0: Initialization failed, possibly due to invalid settings or internal errors.
   */
  virtual int initVideoDecoder(const ExtensionVideoCodecSettings& codec_settings) = 0;
  /**
   * Decodes a frame of encoded video data.
   * @param data The encoded video data to be decoded.
   * @return
   * - 0: Decoding succeeded.
   * - < 0: Decoding failed, possibly due to unsupported data format or internal errors.
   */
  virtual int decode(const ExtensionEncodedImage& data) = 0;
  /**
   * Registers a callback function to handle decoded video frames.
   * @param callback A pointer to the decoder callback interface.
   * @return
   * - 0: Registration succeeded.
   * - < 0: Registration failed, possibly due to an invalid callback pointer.
   */
  virtual int32_t registerDecoderCallback(IExtensionDecoderCallback* callback) = 0;
  /**
   * Releases the resources used by the video decoder.
   * @return
   * - 0: Release succeeded.
   * - < 0: Release failed, possibly due to the decoder not being initialized or internal errors.
   */
  virtual int32_t releaseVideoDecoder() = 0;
  /**
   * Queries the supported codec data formats.
   * @param data_format A pointer to an array where supported pixel formats will be filled.
   * @param size An input/output parameter; initially holds the size of the array, and will be updated with the actual number of formats filled.
   */
  virtual void querySupportCodecDataFormat(media::base::VIDEO_PIXEL_FORMAT* data_format, int& size) = 0;
  
  /**
   * Sets a advanced codec property in the `IExtensionVideoDecoder` class.
   *
   * @param key The pointer to the property name.
   * @param buf The pointer to the buffer of this private property.
   * @param buf_size The buffer size of this private property.
   * @return
   * - The actual size of the private property, if the method call succeeds.
   * - -1, if the method call fails.
   */
  virtual int setAdvancedCodecProperty(const char* key, const char* json_value) = 0;
  /**
   * Gets a advanced codec property in the `IExtensionVideoDecoder` class.
   *
   * @param name The pointer to the property name.
   * @param buf The pointer to the buffer of this advanced codec property.
   * @param buf_size The buffer size of this advanced codec property.
   * @return
   * - The actual size of the advanced codec property, if the method call succeeds.
   * - -1, if the method call fails.
   */
  virtual int getAdvancedCodecProperty(const char* key, char* json_value, int& length) const = 0;
};
}
}
