//
//  Agora Engine SDK
//
//  Created by Sting Feng in 2017-11.
//  Copyright (c) 2017 Agora.io. All rights reserved.

#pragma once  // NOLINT(build/header_guard)

#include <cstring>
#include <stdint.h>
#include <limits>
#include <stddef.h>

#ifndef OPTIONAL_ENUM_SIZE_T
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1800)
#define OPTIONAL_ENUM_SIZE_T enum : size_t
#else
#define OPTIONAL_ENUM_SIZE_T enum
#endif
#endif

namespace agora {
namespace rtc {

typedef unsigned int uid_t;
typedef unsigned int track_id_t;
typedef unsigned int conn_id_t;

static const unsigned int DEFAULT_CONNECTION_ID = 0;
static const unsigned int DUMMY_CONNECTION_ID = (std::numeric_limits<unsigned int>::max)();

struct EncodedVideoFrameInfo;

/**
 * Audio routes.
 */
enum AudioRoute
{
  /**
   * -1: The default audio route.
   */
  ROUTE_DEFAULT = -1,
  /**
   * The headset.
   */
  ROUTE_HEADSET,
  /**
   * The earpiece.
   */
  ROUTE_EARPIECE,
  /**
   * The headset with no microphone.
   */
  ROUTE_HEADSETNOMIC,
  /**
   * The speakerphone.
   */
  ROUTE_SPEAKERPHONE,
  /**
   * The loudspeaker.
   */
  ROUTE_LOUDSPEAKER,
  /**
   * The Bluetooth headset.
   */
  ROUTE_HEADSETBLUETOOTH
};

/**
 * Bytes per sample
 */
enum BYTES_PER_SAMPLE {
  /**
   * two bytes per sample
   */
  TWO_BYTES_PER_SAMPLE = 2,
};

struct AudioParameters {
  int sample_rate;
  size_t channels;
  size_t frames_per_buffer;

  AudioParameters()
      : sample_rate(0),
        channels(0),
        frames_per_buffer(0) {}
};

enum RAW_AUDIO_FRAME_OP_MODE_TYPE {
  /** 0: Read-only mode: Users only read the
     agora::media::IAudioFrameObserver::AudioFrame data without modifying
     anything. For example, when users acquire data with the Agora SDK then push
     the RTMP streams. */
  RAW_AUDIO_FRAME_OP_MODE_READ_ONLY = 0,

  /** 2: Read and write mode: Users read the data from AudioFrame, modify it,
     and then play it. For example, when users have their own sound-effect
     processing module and do some voice pre-processing such as a voice change.
   */
  RAW_AUDIO_FRAME_OP_MODE_READ_WRITE = 2,
};

}  // namespace rtc

namespace media {
namespace base {

typedef void* view_t;

typedef const char* user_id_t;

static const uint8_t kMaxCodecNameLength = 50;

/**
  * The maximum metadata size.
  */
enum MAX_METADATA_SIZE_TYPE {
  MAX_METADATA_SIZE_IN_BYTE = 1024
};

/**
 * The definition of the PacketOptions struct, which contains infomation of the packet
 * in the RTP (Real-time Transport Protocal) header.
 */
struct PacketOptions {
  /**
   * The timestamp of the packet.
   */
  uint32_t timestamp;
  // Audio level indication.
  uint8_t audioLevelIndication;
  PacketOptions()
      : timestamp(0),
        audioLevelIndication(127) {}
};

/**
 * The detailed information of the incoming audio frame in the PCM format.
 */
struct AudioPcmFrame {
  /**
   * The buffer size of the PCM audio frame.
   */
  OPTIONAL_ENUM_SIZE_T {
    // Stereo, 32 kHz, 60 ms (2 * 32 * 60)
    /**
     * The max number of the samples of the data.
     *
     * When the number of audio channel is two, the sample rate is 32 kHZ,
     * the buffer length of the data is 60 ms, the number of the samples of the data is 3840 (2 x 32 x 60).
     */
    kMaxDataSizeSamples = 3840,
    /** The max number of the bytes of the data. */
    kMaxDataSizeBytes = kMaxDataSizeSamples * sizeof(int16_t),
  };

  /** The timestamp (ms) of the audio frame.
   */
  uint32_t capture_timestamp;
  /** The number of samples per channel.
   */
  size_t samples_per_channel_;
  /** The sample rate (Hz) of the audio data.
   */
  int sample_rate_hz_;
  /** The channel number.
   */
  size_t num_channels_;
  /** The number of bytes per sample.
   */
  rtc::BYTES_PER_SAMPLE bytes_per_sample;
  /** The audio frame data. */
  int16_t data_[kMaxDataSizeSamples];

  AudioPcmFrame& operator=(const AudioPcmFrame& src) {
    if(this == &src) {
      return *this;
    }

    this->capture_timestamp = src.capture_timestamp;
    this->samples_per_channel_ = src.samples_per_channel_;
    this->sample_rate_hz_ = src.sample_rate_hz_;
    this->bytes_per_sample = src.bytes_per_sample;
    this->num_channels_ = src.num_channels_;

    size_t length = src.samples_per_channel_ * src.num_channels_;
    if ( length > kMaxDataSizeSamples) {
      length = kMaxDataSizeSamples;
    }

    memcpy(this->data_, src.data_, length * sizeof(int16_t));

    return *this;
  }

  AudioPcmFrame() :
    capture_timestamp(0),
    samples_per_channel_(0),
    sample_rate_hz_(0),
    num_channels_(0),
    bytes_per_sample(rtc::TWO_BYTES_PER_SAMPLE) {
    memset(data_, 0, sizeof(data_));
  }
};

class IAudioFrameObserver {
 public:
  /**
   * Occurs when each time the player receives an audio frame.
   *
   * After registering the audio frame observer,
   * the callback occurs when each time the player receives an audio frame,
   * reporting the detailed information of the audio frame.
   * @param frame The detailed information of the audio frame. See {@link AudioPcmFrame}.
   */
  virtual void onFrame(const AudioPcmFrame* frame) = 0;
  virtual ~IAudioFrameObserver() {}
};

/**
 * Video pixel formats.
 */
enum VIDEO_PIXEL_FORMAT {
  /**
   * 0: Unknown format.
   */
  VIDEO_PIXEL_UNKNOWN = 0,
  /**
   * 1: I420.
   */
  VIDEO_PIXEL_I420 = 1,
  /**
   * 2: BGRA.
   */
  VIDEO_PIXEL_BGRA = 2,
  /**
   * 3: NV21.
   */
  VIDEO_PIXEL_NV21 = 3,
  /**
   * 4: RGBA.
   */
  VIDEO_PIXEL_RGBA = 4,
  /**
   * 8: NV12.
   */
  VIDEO_PIXEL_NV12 = 8,
  /** 
   * 10: GL_TEXTURE_2D
   */
  VIDEO_TEXTURE_2D = 10,
  /**
   * 11: GL_TEXTURE_OES
   */
  VIDEO_TEXTURE_OES = 11,
  /**
   * 16: I422.
   */
  VIDEO_PIXEL_I422 = 16,
};

/**
 * The video display mode.
 */
enum RENDER_MODE_TYPE {
  /**
   * 1: Uniformly scale the video until it fills the visible boundaries
   * (cropped). One dimension of the video may have clipped contents.
   */
  RENDER_MODE_HIDDEN = 1,
  /**
   * 2: Uniformly scale the video until one of its dimension fits the boundary
   * (zoomed to fit). Areas that are not filled due to the disparity in the
   * aspect ratio will be filled with black.
   */
  RENDER_MODE_FIT = 2,
  /**
   * @deprecated
   * 3: This mode is deprecated.
   */
  RENDER_MODE_ADAPTIVE = 3,
};

/**
 * The definition of the ExternalVideoFrame struct.
 */
struct ExternalVideoFrame {
  ExternalVideoFrame()
      : type(VIDEO_BUFFER_RAW_DATA),
        format(VIDEO_PIXEL_UNKNOWN),
        buffer(NULL),
        stride(0),
        height(0),
        cropLeft(0),
        cropTop(0),
        cropRight(0),
        cropBottom(0),
        rotation(0),
        timestamp(0),
        eglContext(NULL),
        eglType(EGL_CONTEXT10),
        textureId(0),
        metadata_buffer(NULL),
        metadata_size(0){}

   /**
   * The EGL context type.
   */
  enum EGL_CONTEXT_TYPE {
    /**
     * 0: When using the OpenGL interface (javax.microedition.khronos.egl.*) defined by Khronos
     */
    EGL_CONTEXT10 = 0,
    /**
     * 0: When using the OpenGL interface (android.opengl.*) defined by Android
     */
    EGL_CONTEXT14 = 1,
  };

  /**
   * Video buffer types.
   */
  enum VIDEO_BUFFER_TYPE {
    /**
     * 1: Raw data.
     */
    VIDEO_BUFFER_RAW_DATA = 1,
    /**
     * 2: The same as VIDEO_BUFFER_RAW_DATA.
     */
    VIDEO_BUFFER_ARRAY = 2,
    /**
     * 3: The video buffer in the format of texture.
     */
    VIDEO_BUFFER_TEXTURE = 3,
  };

  /**
   * The buffer type: #VIDEO_BUFFER_TYPE.
   */
  VIDEO_BUFFER_TYPE type;
  /**
   * The pixel format: #VIDEO_PIXEL_FORMAT
   */
  VIDEO_PIXEL_FORMAT format;
  /**
   * The video buffer.
   */
  void* buffer;
  /**
   * The line spacing of the incoming video frame (px). For
   * texture, it is the width of the texture.
   */
  int stride;
  /**
   * The height of the incoming video frame.
   */
  int height;
  /**
   * [Raw data related parameter] The number of pixels trimmed from the left. The default value is
   * 0.
   */
  int cropLeft;
  /**
   * [Raw data related parameter] The number of pixels trimmed from the top. The default value is
   * 0.
   */
  int cropTop;
  /**
   * [Raw data related parameter] The number of pixels trimmed from the right. The default value is
   * 0.
   */
  int cropRight;
  /**
   * [Raw data related parameter] The number of pixels trimmed from the bottom. The default value
   * is 0.
   */
  int cropBottom;
  /**
   * [Raw data related parameter] The clockwise rotation information of the video frame. You can set the
   * rotation angle as 0, 90, 180, or 270. The default value is 0.
   */
  int rotation;
  /**
   * The timestamp (ms) of the incoming video frame. An incorrect timestamp results in a frame loss or
   * unsynchronized audio and video.
   */
  long long timestamp;
  /** 
   * [Texture-related parameter]
   * When using the OpenGL interface (javax.microedition.khronos.egl.*) defined by Khronos, set EGLContext to this field.
   * When using the OpenGL interface (android.opengl.*) defined by Android, set EGLContext to this field.
   */
  void *eglContext;
  /** 
   * [Texture related parameter] Texture ID used by the video frame.
   */
  EGL_CONTEXT_TYPE eglType;
  /** 
   * [Texture related parameter] Incoming 4 &times; 4 transformational matrix. The typical value is a unit matrix.
   */
  int textureId;
  /**
   * [Texture related parameter] The MetaData buffer.
   *  The default value is NULL
   */
  uint8_t* metadata_buffer;
  /**
   * [Texture related parameter] The MetaData size.
   *  The default value is 0
   */
  int metadata_size;
};

/**
 * The definition of the VideoFrame struct.
 */
struct VideoFrame {
  VideoFrame():
  type(VIDEO_PIXEL_UNKNOWN),
  width(0),
  height(0),
  yStride(0),
  uStride(0),
  vStride(0),
  yBuffer(NULL),
  uBuffer(NULL),
  vBuffer(NULL),
  rotation(0),
  renderTimeMs(0),
  avsync_type(0),
  metadata_buffer(NULL),
  metadata_size(0){}

  /**
   * The video pixel format: #VIDEO_PIXEL_FORMAT.
   */
  VIDEO_PIXEL_FORMAT type;
  /**
   * The width of the video frame.
   */
  int width;
  /**
   * The height of the video frame.
   */
  int height;
  /**
   * The line span of Y buffer in the YUV data.
   */
  int yStride;
  /**
   * The line span of U buffer in the YUV data.
   */
  int uStride;
  /**
   * The line span of V buffer in the YUV data.
   */
  int vStride;
  /**
   * The pointer to the Y buffer in the YUV data.
   */
  uint8_t* yBuffer;
  /**
   * The pointer to the U buffer in the YUV data.
   */
  uint8_t* uBuffer;
  /**
   * The pointer to the V buffer in the YUV data.
   */
  uint8_t* vBuffer;
  /**
   * The clockwise rotation information of this frame. You can set it as 0, 90, 180 or 270.
   */
  int rotation;
  /**
   * The timestamp to render the video stream. Use this parameter for audio-video synchronization when
   * rendering the video.
   *
   * @note This parameter is for rendering the video, not capturing the video.
   */
  int64_t renderTimeMs;
  /**
   * The type of audio-video synchronization.
   */
  int avsync_type;
  /**
   * [Texture related parameter] The MetaData buffer.
   *  The default value is NULL
   */
  uint8_t* metadata_buffer;
  /**
   * [Texture related parameter] The MetaData size.
   *  The default value is 0
   */
  int metadata_size;
};

class IVideoFrameObserver {
 public:
  /**
   * Occurs each time the player receives a video frame.
   *
   * After registering the video frame observer,
   * the callback occurs each time the player receives a video frame to report the detailed information of the video frame.
   * @param frame The detailed information of the video frame. See {@link VideoFrame}.
   */
  virtual void onFrame(const VideoFrame* frame) = 0;
  virtual ~IVideoFrameObserver() {}
  virtual bool isExternal() { return true; }
};

enum MEDIA_PLAYER_SOURCE_TYPE {
  /**
   * The real type of media player when use MEDIA_PLAYER_SOURCE_DEFAULT is decided by the
   * type of SDK package. It is full feature media player in full-featured SDK, or simple
   * media player in others.
   */
  MEDIA_PLAYER_SOURCE_DEFAULT,
  /**
   * Full featured media player is designed to support more codecs and media format, which
   * requires more package size than simple player. If you need this player enabled, you
   * might need to download a full-featured SDK.
   */
  MEDIA_PLAYER_SOURCE_FULL_FEATURED,
  /**
   * Simple media player with limit codec supported, which requires minimal package size
   * requirement and is enabled by default
   */
  MEDIA_PLAYER_SOURCE_SIMPLE,
};

enum VIDEO_MODULE_POSITION {
  POSITION_POST_CAPTURER = 1 << 0,
  POSITION_PRE_RENDERER = 1 << 1,
  POSITION_PRE_ENCODER = 1 << 2,
  POSITION_POST_FILTERS = 1 << 3,
};

}  // namespace base

/**
 * The IAudioFrameObserver class.
 */
class IAudioFrameObserver {
 public:
  /**
   * Audio frame types.
   */
  enum AUDIO_FRAME_TYPE {
    /**
     * 0: 16-bit PCM.
     */
    FRAME_TYPE_PCM16 = 0,
  };
  /**
   * The definition of the AudioFrame struct.
   */
  struct AudioFrame {
    /**
     * The audio frame type: #AUDIO_FRAME_TYPE.
     */
    AUDIO_FRAME_TYPE type;
    /**
     * The number of samples per channel in this frame.
     */
    int samplesPerChannel;
    /**
     * The number of bytes per sample: #BYTES_PER_SAMPLE
     */
    agora::rtc::BYTES_PER_SAMPLE bytesPerSample;
    /**
     * The number of audio channels (data is interleaved, if stereo).
     */
    int channels;
    /**
     * The sample rate of the audio frame.
     */
    int samplesPerSec;
    /**
     * The pointer to the audio data buffer.
     */
    void* buffer;
    /**
     * The timestamp to render the audio data. Use this member for audio-video synchronization when
     * rendering the audio.
     *
     * @note
     * This parameter is the timestamp for audio rendering. Set it as 0.
     */
    int64_t renderTimeMs;
    int avsync_type;

    AudioFrame() : type(FRAME_TYPE_PCM16),
                   samplesPerChannel(0),
                   bytesPerSample(rtc::TWO_BYTES_PER_SAMPLE),
                   channels(0),
                   samplesPerSec(0),
                   buffer(NULL),
                   renderTimeMs(0),
                   avsync_type(0) {}
  };

 public:
  virtual ~IAudioFrameObserver() {}

  /**
   * Occurs when the recorded audio frame is received.
   * @param audioFrame The reference to the audio frame: AudioFrame.
   * @return
   * - true: The recorded audio frame is valid and is encoded and sent.
   * - false: The recorded audio frame is invalid and is not encoded or sent.
   */
  virtual bool onRecordAudioFrame(AudioFrame& audioFrame) = 0;
  /**
   * Occurs when the playback audio frame is received.
   * @param audioFrame The reference to the audio frame: AudioFrame.
   * @return
   * - true: The playback audio frame is valid and is encoded and sent.
   * - false: The playback audio frame is invalid and is not encoded or sent.
   */
  virtual bool onPlaybackAudioFrame(AudioFrame& audioFrame) = 0;
  /**
   * Occurs when the mixed audio data is received.
   * @param audioFrame The reference to the audio frame: AudioFrame.
   * @return
   * - true: The mixed audio data is valid and is encoded and sent.
   * - false: The mixed audio data is invalid and is not encoded or sent.
   */
  virtual bool onMixedAudioFrame(AudioFrame& audioFrame) = 0;
  /**
   * Occurs when the before-mixing playback audio frame is received.
   * @param uid ID of the remote user.
   * @param audioFrame The reference to the audio frame: AudioFrame.
   * @return
   * - true: The before-mixing playback audio frame is valid and is encoded and sent.
   * - false: The before-mixing playback audio frame is invalid and is not encoded or sent.
   */
  virtual bool onPlaybackAudioFrameBeforeMixing(unsigned int uid, AudioFrame& audioFrame) = 0;
};

struct AudioSpectrumData {
  /**
   * The audio spectrum data of audio.
   */
  const float *audioSpectrumData;
  /**
   * The data length of audio spectrum data.
   */
  int dataLength;

  AudioSpectrumData() : audioSpectrumData(NULL), dataLength(0) {}
  AudioSpectrumData(const float *data, int length) :
    audioSpectrumData(data), dataLength(length) {}
};

struct UserAudioSpectrumInfo  {
  /**
   * User ID of the speaker.
   */
  agora::rtc::uid_t uid;
  /**
   * The audio spectrum data of audio.
   */
  struct AudioSpectrumData spectrumData;

  UserAudioSpectrumInfo () : uid(0), spectrumData() {}
  UserAudioSpectrumInfo(agora::rtc::uid_t _uid, const float *data, int length) :
    uid(_uid) { spectrumData.audioSpectrumData = data; spectrumData.dataLength = length; }
};

/**
 * The IAudioSpectrumObserver class.
 */
class IAudioSpectrumObserver {
public:
  virtual ~IAudioSpectrumObserver() {}

  /**
   * Reports the audio spectrum of local audio.
   *
   * This callback reports the audio spectrum data of the local audio at the moment
   * in the channel.
   *
   * You can set the time interval of this callback using \ref ILocalUser::enableAudioSpectrumMonitor "enableAudioSpectrumMonitor".
   *
   * @param data The audio spectrum data of local audio.
   * - true: Processed.
   * - false: Not processed.
   */
  virtual bool onLocalAudioSpectrum(const AudioSpectrumData& data) = 0;

  /**
   * Reports the audio spectrum of remote user.
   *
   * This callback reports the IDs and audio spectrum data of the loudest speakers at the moment
   * in the channel.
   *
   * You can set the time interval of this callback using \ref ILocalUser::enableAudioSpectrumMonitor "enableAudioSpectrumMonitor".
   *
   * @param spectrums The pointer to \ref agora::media::UserAudioSpectrumInfo "UserAudioSpectrumInfo", which is an array containing
   * the user ID and audio spectrum data for each speaker.
   * - This array contains the following members:
   *   - `uid`, which is the UID of each remote speaker
   *   - `spectrumData`, which reports the audio spectrum of each remote speaker.
   * @param spectrumNumber The array length of the spectrums.
   * - true: Processed.
   * - false: Not processed.
   */
  virtual bool onRemoteAudioSpectrum(const UserAudioSpectrumInfo * spectrums, unsigned int spectrumNumber) = 0;
};

/**
 * The IVideoEncodedFrameObserver class.
 */
class IVideoEncodedFrameObserver {
 public:
  /**
   * Occurs each time the SDK receives an encoded video image.
   * @param uid The user id of remote user.
   * @param imageBuffer The pointer to the video image buffer.
   * @param length The data length of the video image.
   * @param videoEncodedFrameInfo The information of the encoded video frame: EncodedVideoFrameInfo.
   * @return Determines whether to accept encoded video image.
   * - true: Accept.
   * - false: Do not accept.
   */
  virtual bool OnEncodedVideoFrame(rtc::uid_t uid, const uint8_t* imageBuffer, size_t length,
                                   const rtc::EncodedVideoFrameInfo& videoEncodedFrameInfo) = 0;

  virtual ~IVideoEncodedFrameObserver() {}
};

/**
 * The IVideoFrameObserver class.
 */
class IVideoFrameObserver {
 public:
  typedef media::base::VideoFrame VideoFrame;

  enum VIDEO_FRAME_PROCESS_MODE {
    PROCESS_MODE_READ_ONLY, // Observer works as a pure renderer and will not modify the original frame.
    PROCESS_MODE_READ_WRITE, // Observer works as a filter that will process the video frame and affect the following frame processing in SDK.
  };

 public:
  virtual ~IVideoFrameObserver() {}

  /**
   * Occurs each time the SDK receives a video frame captured by the local camera.
   *
   * After you successfully register the video frame observer, the SDK triggers this callback each time
   * a video frame is received. In this callback, you can get the video data captured by the local
   * camera. You can then pre-process the data according to your scenarios.
   *
   * After pre-processing, you can send the processed video data back to the SDK by setting the
   * `videoFrame` parameter in this callback.
   *
   * @param videoFrame A pointer to the video frame: VideoFrame
   * @return Determines whether to ignore the current video frame if the pre-processing fails:
   * - true: Do not ignore.
   * - false: Ignore, in which case this method does not sent the current video frame to the SDK.
  */
  virtual bool onCaptureVideoFrame(VideoFrame& videoFrame) = 0;

  virtual bool onSecondaryCameraCaptureVideoFrame(VideoFrame& videoFrame) = 0;

  /**
   * Occurs each time the SDK receives a video frame captured by the screen.
   *
   * After you successfully register the video frame observer, the SDK triggers this callback each time
   * a video frame is received. In this callback, you can get the video data captured by the screen.
   * You can then pre-process the data according to your scenarios.
   *
   * After pre-processing, you can send the processed video data back to the SDK by setting the
   * `videoFrame` parameter in this callback.
   *
   * @param videoFrame A pointer to the video frame: VideoFrame
   * @return Determines whether to ignore the current video frame if the pre-processing fails:
   * - true: Do not ignore.
   * - false: Ignore, in which case this method does not sent the current video frame to the SDK.
   */
  virtual bool onScreenCaptureVideoFrame(VideoFrame& videoFrame) = 0;
  /**
   * Occurs each time the SDK receives a video frame decoded by the MediaPlayer.
   *
   * After you successfully register the video frame observer, the SDK triggers this callback each
   * time a video frame is decoded. In this callback, you can get the video data decoded by the
   * MediaPlayer. You can then pre-process the data according to your scenarios.
   *
   * After pre-processing, you can send the processed video data back to the SDK by setting the
   * `videoFrame` parameter in this callback.
   *
   * @param videoFrame A pointer to the video frame: VideoFrame
   * @param mediaPlayerId of the mediaPlayer.
   * @return Determines whether to ignore the current video frame if the pre-processing fails:
   * - true: Do not ignore.
   * - false: Ignore, in which case this method does not sent the current video frame to the SDK.
   */
  virtual bool onMediaPlayerVideoFrame(VideoFrame& videoFrame, int mediaPlayerId) = 0;

  virtual bool onSecondaryScreenCaptureVideoFrame(VideoFrame& videoFrame) = 0;

  /**
   * Occurs each time the SDK receives a video frame sent by the remote user.
   *
   * After you successfully register the video frame observer, the SDK triggers this callback each time a
   * video frame is received. In this callback, you can get the video data sent by the remote user. You
   * can then post-process the data according to your scenarios.
   *
   * After post-processing, you can send the processed data back to the SDK by setting the `videoFrame`
   * parameter in this callback.
   *
   * @param uid ID of the remote user who sends the current video frame.
   * @param connectionId ID of the connection.
   * @param videoFrame A pointer to the video frame: VideoFrame
   * @return Determines whether to ignore the current video frame if the post-processing fails:
   * - true: Do not ignore.
   * - false: Ignore, in which case this method does not sent the current video frame to the SDK.
   */
  virtual bool onRenderVideoFrame(rtc::uid_t uid, rtc::conn_id_t connectionId,
                                  VideoFrame& videoFrame) = 0;

  virtual bool onTranscodedVideoFrame(VideoFrame& videoFrame) = 0;

  /**
   * Indicate the video frame mode of the observer.
   * @return VIDEO_FRAME_PROCESS_MODE
   */
  virtual VIDEO_FRAME_PROCESS_MODE getVideoFrameProcessMode() {
    return PROCESS_MODE_READ_ONLY;
  }

  /*
   * Occurs each time needs to get preference video frame type.
   *
   * @return preference video pixel format.
   */
  virtual base::VIDEO_PIXEL_FORMAT getVideoPixelFormatPreference() { return base::VIDEO_PIXEL_I420; }

  /**
   * Occurs each time needs to get rotation angle.
   *
   * @return rotation angle.
   */
  virtual int getRotationApplied() { return 0; }

  /**
   * Occurs each time needs to get whether mirror is applied or not.
   *
   * @return Determines whether to mirror.
   * - true: need to mirror.
   * - false: no mirror.
   */
  virtual bool getMirrorApplied() { return false; }

  /**
   * Indicate if the observer is for internal use.
   * Note: Never override this function
   * @return
   * - true: the observer is for external use
   * - false: the observer is for internal use
   */
  virtual bool isExternal() { return true; }
};

}  // namespace media
}  // namespace agora
