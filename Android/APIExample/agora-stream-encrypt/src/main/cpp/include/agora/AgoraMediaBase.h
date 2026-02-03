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

#if !defined(__APPLE__)
#define __deprecated
#endif

namespace agora {
namespace rtc {

typedef unsigned int uid_t;
typedef unsigned int track_id_t;
typedef unsigned int conn_id_t;
typedef unsigned int video_track_id_t;

static const unsigned int INVALID_TRACK_ID = 0xffffffff;
static const unsigned int DEFAULT_CONNECTION_ID = 0;
static const unsigned int DUMMY_CONNECTION_ID = (std::numeric_limits<unsigned int>::max)();

struct EncodedVideoFrameInfo;
/**
 * @brief The context information of the extension.
 */
struct ExtensionContext {
    /**
     * Whether the `uid` in `ExtensionContext` is valid:
     * - `true`: The `uid` is valid.
     * - `false`: The `uid` is invalid.
     */
    bool isValid;
    /**
     * The user ID. 0 represents a local user, while greater than 0 represents a remote user.
     */
    uid_t uid;
    /**
     * The name of the extension provider.
     */
    const char *providerName;
    /**
     * The name of the extension.
     */
    const char *extensionName;
    ExtensionContext():isValid(false), uid(0), providerName(NULL), extensionName(NULL) {}
};


/**
 * @brief The type of the video source.
 */
enum VIDEO_SOURCE_TYPE {
  /**
   * 0: (Default) The primary camera.
   */
  VIDEO_SOURCE_CAMERA_PRIMARY = 0,
  /**
   * 0: (Default) The primary camera.
   */
  VIDEO_SOURCE_CAMERA = VIDEO_SOURCE_CAMERA_PRIMARY,
  /**
   * 1: The secondary camera.
   */
  VIDEO_SOURCE_CAMERA_SECONDARY = 1,
  /**
   * 2: The primary screen.
   */
  VIDEO_SOURCE_SCREEN_PRIMARY = 2,
  /**
   * 2: The primary screen.
   */
  VIDEO_SOURCE_SCREEN = VIDEO_SOURCE_SCREEN_PRIMARY,
  /**
   * 3: The secondary screen.
   */
  VIDEO_SOURCE_SCREEN_SECONDARY = 3,
  /**
   * 4: A custom video source.
   */
  VIDEO_SOURCE_CUSTOM = 4,
  /**
   * 5: The media player.
   */
  VIDEO_SOURCE_MEDIA_PLAYER = 5,
  /**
   * 6: One PNG image.
   */
  VIDEO_SOURCE_RTC_IMAGE_PNG = 6,
  /**
   * 7: One JPEG image.
   */
  VIDEO_SOURCE_RTC_IMAGE_JPEG = 7,
  /**
   * 8: One GIF image.
   */
  VIDEO_SOURCE_RTC_IMAGE_GIF = 8,
  /**
   * 9: One remote video acquired by the network.
   */
  VIDEO_SOURCE_REMOTE = 9,
  /**
   * 10: One transcoded video source.
   */
  VIDEO_SOURCE_TRANSCODED = 10,

  /**
   * 11: (For Android, Windows, and macOS only) The third camera.
   */
  VIDEO_SOURCE_CAMERA_THIRD = 11,
  /**
   * 12: (For Android, Windows, and macOS only) The fourth camera.
   */
  VIDEO_SOURCE_CAMERA_FOURTH = 12,
  /**
   * 13: (For Windows and macOS only) The third screen.
   */
  VIDEO_SOURCE_SCREEN_THIRD = 13,
  /**
   * 14: (For Windows and macOS only) The fourth screen.
   */
  VIDEO_SOURCE_SCREEN_FOURTH = 14,
  /**
   * 15: Video processed by the speech driven extension.
   */
  VIDEO_SOURCE_SPEECH_DRIVEN = 15,

  /**
   * 100: An unknown video source.
   */
  VIDEO_SOURCE_UNKNOWN = 100
};

/**
 * @brief The audio source type.
 */
enum AUDIO_SOURCE_TYPE {
  /**
   * 0: (Default) Microphone.
   */
  AUDIO_SOURCE_MICROPHONE = 0,
  /**
   * 1: Custom audio stream.
   */
  AUDIO_SOURCE_CUSTOM = 1,
  /**
   * 2: Media player.
   */
  AUDIO_SOURCE_MEDIA_PLAYER = 2,
  /**
   * 3: System audio stream captured during screen sharing.
   */
  AUDIO_SOURCE_LOOPBACK_RECORDING = 3,
  /** Audio captured by mixed source.
   */
  AUDIO_SOURCE_MIXED_STREAM = 4,
  /**
   * 5: Audio stream from a specified remote user.
   */
  AUDIO_SOURCE_REMOTE_USER = 5,
  /**
   * 6: Mixed audio streams from all users in the current channel.
   */
  AUDIO_SOURCE_REMOTE_CHANNEL = 6,

  /**
   * 100: An unknown audio source.
   */
  AUDIO_SOURCE_UNKNOWN = 100
};

/**
 * @brief The type of the audio route.
 */
enum AudioRoute {
  /**
   * -1: The default audio route.
   */
  ROUTE_DEFAULT = -1,
  /**
   * 0: Audio output routing is a headset with microphone.
   */
  ROUTE_HEADSET = 0,
  /**
   * 1: The audio route is an earpiece.
   */
  ROUTE_EARPIECE = 1,
  /**
   * 2: The audio route is a headset without a microphone.
   */
  ROUTE_HEADSETNOMIC = 2,
  /**
   * 3: The audio route is the speaker that comes with the device.
   */
  ROUTE_SPEAKERPHONE = 3,
  /**
   * 4: The audio route is an external speaker. (iOS and macOS only)
   */
  ROUTE_LOUDSPEAKER = 4,
  /**
   * 5: The audio route is a Bluetooth device using the HFP protocol.
   */
  ROUTE_BLUETOOTH_DEVICE_HFP = 5,
  /**
   * 6: The audio route is a USB peripheral device. (For macOS only)
   */
  ROUTE_USB = 6,
  /**
   * 7: The audio route is an HDMI peripheral device. (For macOS only)
   */
  ROUTE_HDMI = 7,
  /**
   * 8: The audio route is a DisplayPort peripheral device. (For macOS only)
   */
  ROUTE_DISPLAYPORT = 8,
  /**
   * 9: The audio route is Apple AirPlay. (For macOS only)
   */
  ROUTE_AIRPLAY = 9,
  /**
   * 10: The audio route is a Bluetooth device using the A2DP protocol.
   */
  ROUTE_BLUETOOTH_DEVICE_A2DP = 10,
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

  AudioParameters() : sample_rate(0), channels(0), frames_per_buffer(0) {}
};

/**
 * @brief The use mode of the audio data.
 */
enum RAW_AUDIO_FRAME_OP_MODE_TYPE {
  /**
   * 0: Read-only mode, the user only read the original data from `AudioFrame` without any
   * modification. For example, when users acquire the data with the Agora SDK, then start the media
   * push.
   */
  RAW_AUDIO_FRAME_OP_MODE_READ_ONLY = 0,

  /**
   * 2: Read and write mode, Users read the data from `AudioFrame`, modify it, and then play it. For
   * example, when users have their own audio-effect processing module and perform some voice
   * preprocessing, such as a voice change.
   */
  RAW_AUDIO_FRAME_OP_MODE_READ_WRITE = 2,
};

}  // namespace rtc

namespace media {
/**
 * @brief Media source type.
 */
enum MEDIA_SOURCE_TYPE {
  /**
   * 0: Audio playback device.
   */
  AUDIO_PLAYOUT_SOURCE = 0,
  /**
   * 1: Audio capturing device.
   */
  AUDIO_RECORDING_SOURCE = 1,
  /**
   * 2: The primary camera.
   */
  PRIMARY_CAMERA_SOURCE = 2,
  /**
   * 3: A secondary camera.
   */
  SECONDARY_CAMERA_SOURCE = 3,
  /**
   * 4: Video captured by primary screen capturer.
   */
  PRIMARY_SCREEN_SOURCE = 4,
  /**
   * 5: Video captured by secondary screen capturer.
   */
  SECONDARY_SCREEN_SOURCE = 5,
  /**
   * 6: Custom video source.
   */
  CUSTOM_VIDEO_SOURCE = 6,
  /**
   * 7: Video for media player sharing.
   */
  MEDIA_PLAYER_SOURCE = 7,
  /**
   * 8: Video for png image.
   */
  RTC_IMAGE_PNG_SOURCE = 8,
  /**
   * 9: Video for jpeg image.
   */
  RTC_IMAGE_JPEG_SOURCE = 9,
  /**
   * 10: Video for gif image.
   */
  RTC_IMAGE_GIF_SOURCE = 10,
  /**
   * 11: Remote video received from network.
   */
  REMOTE_VIDEO_SOURCE = 11,
  /**
   * 12: Video for transcoded.
   */
  TRANSCODED_VIDEO_SOURCE = 12,
  /**
   * 13: Video processed by the speech driven extension.
   */
  SPEECH_DRIVEN_VIDEO_SOURCE = 13,
  /**
   * 100: Unknown media source.
   */
  UNKNOWN_MEDIA_SOURCE = 100
};

namespace base {

typedef void* view_t;

typedef const char* user_id_t;

static const uint8_t kMaxCodecNameLength = 50;

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
  PacketOptions() : timestamp(0), audioLevelIndication(127) {}
};

/**
 * The detailed information of the incoming audio encoded frame.
 */

struct AudioEncodedFrameInfo {
  /**
   * The send time of the packet.
   */
  uint64_t sendTs;
  /**
   * The codec of the packet.
   */
  uint8_t codec;
  AudioEncodedFrameInfo() : sendTs(0), codec(0) {}
};

/**
 * @brief The parameters of the audio frame in PCM format.
 */
struct AudioPcmFrame {
  /**
   * The buffer size of the PCM audio frame. The initial value is 3840.
   */
  OPTIONAL_ENUM_SIZE_T{
      // Stereo, 32 kHz, 60 ms (2 * 32 * 60)
      /**
       * The max number of the samples of the data.
       *
       * When the number of audio channel is two, the sample rate is 32 kHZ,
       * the buffer length of the data is 60 ms, the number of the samples of the data is 3840 (2 x
       * 32 x 60).
       */
      kMaxDataSizeSamples = 3840,
      /** The max number of the bytes of the data. */
      kMaxDataSizeBytes = kMaxDataSizeSamples * sizeof(int16_t),
  };

  /**
   * The timestamp (ms) of the audio frame.
   */
  int64_t capture_timestamp;
  /**
   * The number of samples per channel in the audio frame.
   */
  size_t samples_per_channel_;
  /**
   * Audio sample rate (Hz).
   */
  int sample_rate_hz_;
  /**
   * The number of audio channels.
   */
  size_t num_channels_;
  /**  @technical preview
   * The audio track number. if mpk enableMultiAudioTrack, audio frame will have audio track number, eg 0 or 1.
   */
  int audio_track_number_;
  /**
   * The number of bytes per sample.
   */
  rtc::BYTES_PER_SAMPLE bytes_per_sample;
  /**
   * The audio frame.
   */
  int16_t data_[kMaxDataSizeSamples];
  
  /** 
   *  @technical preview
   *  data_[kMaxDataSizeSamples] is real stereo data 
   */
  bool is_stereo_;

  AudioPcmFrame& operator=(const AudioPcmFrame& src) {
    if (this == &src) {
      return *this;
    }

    capture_timestamp = src.capture_timestamp;
    samples_per_channel_ = src.samples_per_channel_;
    sample_rate_hz_ = src.sample_rate_hz_;
    bytes_per_sample = src.bytes_per_sample;
    num_channels_ = src.num_channels_;
    is_stereo_ = src.is_stereo_;
    this->audio_track_number_ = src.audio_track_number_;

    size_t length = src.samples_per_channel_ * src.num_channels_;
    if (length > kMaxDataSizeSamples) {
      length = kMaxDataSizeSamples;
    }

    memcpy(data_, src.data_, length * sizeof(int16_t));

    return *this;
  }

  AudioPcmFrame()
      : capture_timestamp(0),
        samples_per_channel_(0),
        sample_rate_hz_(0),
        num_channels_(0),
        audio_track_number_(0),
        bytes_per_sample(rtc::TWO_BYTES_PER_SAMPLE),
        is_stereo_(false) {
    memset(data_, 0, sizeof(data_));
  }

  AudioPcmFrame(const AudioPcmFrame& src)
      : capture_timestamp(src.capture_timestamp),
        samples_per_channel_(src.samples_per_channel_),
        sample_rate_hz_(src.sample_rate_hz_),
        num_channels_(src.num_channels_),
        audio_track_number_(src.audio_track_number_),
        bytes_per_sample(src.bytes_per_sample),
        is_stereo_(src.is_stereo_) {
    size_t length = src.samples_per_channel_ * src.num_channels_;
    if (length > kMaxDataSizeSamples) {
      length = kMaxDataSizeSamples;
    }

    memcpy(this->data_, src.data_, length * sizeof(int16_t));
  }
};

/**
 * @brief The channel mode.
 */
enum AUDIO_DUAL_MONO_MODE {
  /**
   * 0: Original mode.
   */
  AUDIO_DUAL_MONO_STEREO = 0,
  /**
   * 1: Left channel mode. This mode replaces the audio of the right channel with the audio of the
   * left channel, which means the user can only hear the audio of the left channel.
   */
  AUDIO_DUAL_MONO_L = 1,
  /**
   * 2: Right channel mode. This mode replaces the audio of the left channel with the audio of the
   * right channel, which means the user can only hear the audio of the right channel.
   */
  AUDIO_DUAL_MONO_R = 2,
  /**
   * 3: Mixed channel mode. This mode mixes the audio of the left channel and the right channel, which
   * means the user can hear the audio of the left channel and the right channel at the same time.
   */
  AUDIO_DUAL_MONO_MIX = 3
};

/**
 * @brief The video pixel format.
 */
enum VIDEO_PIXEL_FORMAT {
  /**
   * 0: Raw video pixel format.
   */
  VIDEO_PIXEL_DEFAULT = 0,
  /**
   * 1: The format is I420.
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
   * 4: The format is RGBA.
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
  /*
  12: pixel format for iOS CVPixelBuffer NV12
  */
  VIDEO_CVPIXEL_NV12 = 12,
  /*
  13: pixel format for iOS CVPixelBuffer I420
  */
  VIDEO_CVPIXEL_I420 = 13,
  /*
  14: pixel format for iOS CVPixelBuffer BGRA
  */
  VIDEO_CVPIXEL_BGRA = 14,
  /**
  15: pixel format for iOS CVPixelBuffer P010(10bit NV12)
  */
  VIDEO_CVPIXEL_P010 = 15,
  /**
   * 16: The format is I422.
   */
  VIDEO_PIXEL_I422 = 16,
  /**
   * 17: The ID3D11TEXTURE2D format. Currently supported types are `DXGI_FORMAT_B8G8R8A8_UNORM`,
   * `DXGI_FORMAT_B8G8R8A8_TYPELESS` and `DXGI_FORMAT_NV12`.
   */
  VIDEO_TEXTURE_ID3D11TEXTURE2D = 17,
  /**
   * 18: I010. 10bit I420 data.
   * @technical preview
   */
  VIDEO_PIXEL_I010 = 18,
};

/**
 * @brief Video display modes.
 */
enum RENDER_MODE_TYPE {
  /**
   * 1: Hidden mode. The priority is to fill the window. Any excess video that does not match the
   * window size will be cropped.
   */
  RENDER_MODE_HIDDEN = 1,
  /**
   * 2: Fit mode. The priority is to ensure that all video content is displayed. Any areas of the
   * window that are not filled due to the mismatch between video size and window size will be filled
   * with black.
   */
  RENDER_MODE_FIT = 2,
  /**
   * 3: Adaptive mode.
   * @deprecated
   */
  RENDER_MODE_ADAPTIVE __deprecated = 3,
};

/**
 * The camera video source type
 */
enum CAMERA_VIDEO_SOURCE_TYPE {
  /**
   * 0: the video frame comes from the front camera
   */
  CAMERA_SOURCE_FRONT = 0,
  /**
   * 1: the video frame comes from the back camera
   */
  CAMERA_SOURCE_BACK = 1,
  /**
   * 1: the video frame source is unsepcified
   */
  VIDEO_SOURCE_UNSPECIFIED = 2,
};

/**
 * The IVideoFrameMetaInfo class.
 * This interface provides access to metadata information.
 */
class IVideoFrameMetaInfo {
 public:
  enum META_INFO_KEY {
    KEY_FACE_CAPTURE = 0,
  };
  virtual ~IVideoFrameMetaInfo(){};
  virtual const char* getMetaInfoStr(META_INFO_KEY key) const = 0;
};

struct ColorSpace {
  enum PrimaryID {
    // The indices are equal to the values specified in T-REC H.273 Table 2.
    PRIMARYID_BT709 = 1,
    PRIMARYID_UNSPECIFIED = 2,
    PRIMARYID_BT470M = 4,
    PRIMARYID_BT470BG = 5,
    PRIMARYID_SMPTE170M = 6,  // Identical to BT601
    PRIMARYID_SMPTE240M = 7,
    PRIMARYID_FILM = 8,
    PRIMARYID_BT2020 = 9,
    PRIMARYID_SMPTEST428 = 10,
    PRIMARYID_SMPTEST431 = 11,
    PRIMARYID_SMPTEST432 = 12,
    PRIMARYID_JEDECP22 = 22,  // Identical to EBU3213-E
  };

  enum RangeID {
    // The indices are equal to the values specified at
    // https://www.webmproject.org/docs/container/#colour for the element Range.
    RANGEID_INVALID = 0,
    // Limited Rec. 709 color range with RGB values ranging from 16 to 235.
    RANGEID_LIMITED = 1,
    // Full RGB color range with RGB valees from 0 to 255.
    RANGEID_FULL = 2,
    // Range is defined by MatrixCoefficients/TransferCharacteristics.
    RANGEID_DERIVED = 3,
  };

  enum MatrixID {
    // The indices are equal to the values specified in T-REC H.273 Table 4.
    MATRIXID_RGB = 0,
    MATRIXID_BT709 = 1,
    MATRIXID_UNSPECIFIED = 2,
    MATRIXID_FCC = 4,
    MATRIXID_BT470BG = 5,
    MATRIXID_SMPTE170M = 6,
    MATRIXID_SMPTE240M = 7,
    MATRIXID_YCOCG = 8,
    MATRIXID_BT2020_NCL = 9,
    MATRIXID_BT2020_CL = 10,
    MATRIXID_SMPTE2085 = 11,
    MATRIXID_CDNCLS = 12,
    MATRIXID_CDCLS = 13,
    MATRIXID_BT2100_ICTCP = 14,
  };

  enum TransferID {
    // The indices are equal to the values specified in T-REC H.273 Table 3.
    TRANSFERID_BT709 = 1,
    TRANSFERID_UNSPECIFIED = 2,
    TRANSFERID_GAMMA22 = 4,
    TRANSFERID_GAMMA28 = 5,
    TRANSFERID_SMPTE170M = 6,
    TRANSFERID_SMPTE240M = 7,
    TRANSFERID_LINEAR = 8,
    TRANSFERID_LOG = 9,
    TRANSFERID_LOG_SQRT = 10,
    TRANSFERID_IEC61966_2_4 = 11,
    TRANSFERID_BT1361_ECG = 12,
    TRANSFERID_IEC61966_2_1 = 13,
    TRANSFERID_BT2020_10 = 14,
    TRANSFERID_BT2020_12 = 15,
    TRANSFERID_SMPTEST2084 = 16,
    TRANSFERID_SMPTEST428 = 17,
    TRANSFERID_ARIB_STD_B67 = 18,
  };

  PrimaryID primaries;
  TransferID transfer;
  MatrixID matrix;
  RangeID range;

  ColorSpace()
      : primaries(PRIMARYID_UNSPECIFIED), transfer(TRANSFERID_UNSPECIFIED),
        matrix(MATRIXID_UNSPECIFIED), range(RANGEID_INVALID) {}

  bool validate() const {
    return primaries != PRIMARYID_UNSPECIFIED || transfer != TRANSFERID_UNSPECIFIED ||
           matrix != MATRIXID_UNSPECIFIED ||
           range != RANGEID_INVALID;
  }
};

/**
 * The definition of the Hdr10MetadataInfo struct.
 */
struct Hdr10MetadataInfo {
   /**
   * The x coordinates of the red value in the CIE1931 color space. The values need to normalized to 50,000.
   */
  uint16_t redPrimaryX;
   /**
   * The y coordinates of the red value in the CIE1931 color space. The values need to normalized to 50,000.
   */
  uint16_t redPrimaryY;
   /**
   * The x coordinates of the green value in the CIE1931 color space. The values need to normalized to 50,000.
   */
  uint16_t greenPrimaryX;
   /**
   * The y coordinates of the green value in the CIE1931 color space. The values need to normalized to 50,000.
   */
  uint16_t greenPrimaryY;
   /**
   * The x coordinates of the blue value in the CIE1931 color space. The values need to normalized to 50,000.
   */
  uint16_t bluePrimaryX;
   /**
   * The y coordinates of the blue value in the CIE1931 color space. The values need to normalized to 50,000.
   */
  uint16_t bluePrimaryY;
   /**
   * The x coordinates of the white point in the CIE1931 color space.The values need to normalized to 50,000.
   */
  uint16_t whitePointX;
   /**
   * The y coordinates of the white point in the CIE1931 color space.The values need to normalized to 50,000.
   */
  uint16_t whitePointY;
   /**
   * The maximum number of nits of the display used to master the content. The values need to normalized to 10,000.
   */
  unsigned int maxMasteringLuminance;
   /**
   * The minimum number of nits of the display used to master the content. The values need to normalized to 10,000.
   */
  unsigned int minMasteringLuminance;
   /**
   * The maximum content light level (MaxCLL). This is the nit value corresponding to the brightest pixel used anywhere in the content.
   */
  uint16_t maxContentLightLevel;
   /**
   * The maximum frame average light level (MaxFALL). This is the nit value corresponding to the average luminance of the frame which has the brightest average luminance anywhere in the content.
   */
  uint16_t maxFrameAverageLightLevel;

  Hdr10MetadataInfo()
      : redPrimaryX(0),
        redPrimaryY(0),
        greenPrimaryX(0),
        greenPrimaryY(0),
        bluePrimaryX(0),
        bluePrimaryY(0),
        whitePointX(0),
        whitePointY(0),
        maxMasteringLuminance(0),
        minMasteringLuminance(0),
        maxContentLightLevel(0),
        maxFrameAverageLightLevel(0){}

  bool validate() const {
    return maxContentLightLevel >= 0 && maxContentLightLevel <= 20000 &&
           maxFrameAverageLightLevel >= 0 &&
           maxFrameAverageLightLevel <= 20000;
  }
};

/**
 * @brief The relative position of `alphaBuffer` and video frames.
 */
enum ALPHA_STITCH_MODE {
  /**
   * 0: (Default) Only video frame, that is, `alphaBuffer` is not stitched with the video frame.
   */
  NO_ALPHA_STITCH = 0,
  /**
   * 1: `alphaBuffer` is above the video frame.
   */
  ALPHA_STITCH_UP = 1,
  /**
   * 2: `alphaBuffer` is below the video frame.
   */
  ALPHA_STITCH_BELOW = 2,
  /**
   * 3: `alphaBuffer` is to the left of the video frame.
   */
  ALPHA_STITCH_LEFT = 3,
  /**
   * 4: `alphaBuffer` is to the right of the video frame.
   */
  ALPHA_STITCH_RIGHT = 4,
};


/**
 * @brief The external video frame.
 */
struct ExternalVideoFrame {
  ExternalVideoFrame()
      : type(VIDEO_BUFFER_RAW_DATA),
        format(VIDEO_PIXEL_DEFAULT),
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
        fenceObject(0),
        metadataBuffer(NULL),
        metadataSize(0),
        alphaBuffer(NULL),
        fillAlphaBuffer(false),
        alphaStitchMode(NO_ALPHA_STITCH),
        d3d11Texture2d(NULL),
        textureSliceIndex(0){}

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
   * @brief The video buffer type.
   */
  enum VIDEO_BUFFER_TYPE {
    /**
     * 1: The video buffer in the format of raw data.
     */
    VIDEO_BUFFER_RAW_DATA = 1,
    /**
     * 2: The video buffer in the format of raw data.
     */
    VIDEO_BUFFER_ARRAY = 2,
    /**
     * 3: The video buffer in the format of `Texture`.
     */
    VIDEO_BUFFER_TEXTURE = 3,
  };

  /**
   * The video type. See `VIDEO_BUFFER_TYPE`.
   */
  VIDEO_BUFFER_TYPE type;
  /**
   * The pixel format. See `VIDEO_PIXEL_FORMAT`.
   */
  VIDEO_PIXEL_FORMAT format;

  /**
   * Video frame buffer.
   */
  void* buffer;
  /**
   * Line spacing of the incoming video frame, which must be in pixels instead of bytes. For textures,
   * it is the width of the texture.
   */
  int stride;
  /**
   * Height of the incoming video frame.
   */
  int height;
  /**
   * Raw data related parameter. The number of pixels trimmed from the left. The default value is 0.
   */
  int cropLeft;
  /**
   * Raw data related parameter. The number of pixels trimmed from the top. The default value is 0.
   */
  int cropTop;
  /**
   * Raw data related parameter. The number of pixels trimmed from the right. The default value is 0.
   */
  int cropRight;
  /**
   * Raw data related parameter. The number of pixels trimmed from the bottom. The default value is 0.
   */
  int cropBottom;
  /**
   * Raw data related parameter. The clockwise rotation of the video frame. You can set the rotation
   * angle as 0, 90, 180, or 270. The default value is 0.
   */
  int rotation;
  /**
   * Timestamp (ms) of the incoming video frame. An incorrect timestamp results in frame loss or
   * unsynchronized audio and video.
   */
  long long timestamp;
  /**
   * This parameter only applies to video data in Texture format.
   * - When using the OpenGL interface (javax.microedition.khronos.egl.*) defined by Khronos, set
   * eglContext to this field.
   * - When using the OpenGL interface (android.opengl.*) defined by Android, set eglContext to this
   * field.
   */
  void* eglContext;
  /**
   * This parameter only applies to video data in Texture format. Texture ID of the video frame.
   */
  EGL_CONTEXT_TYPE eglType;
  /**
   * This parameter only applies to video data in Texture format. Incoming 4 × 4 transformational
   * matrix. The typical value is a unit matrix.
   */
  int textureId;
  /**
   * [Texture related parameter] The fence object related to the textureId parameter, indicating the synchronization status of the video data in Texture format.
   * The default value is 0
   */
  long long fenceObject; 
  /**
   * [Texture related parameter] Incoming 4 &times; 4 transformational matrix. The typical value is a unit matrix.
   */
  float matrix[16];
  /**
   * This parameter only applies to video data in Texture format. The MetaData buffer. The default
   * value is `NULL`.
   */
  uint8_t* metadataBuffer;
  /**
   * This parameter only applies to video data in Texture format. The MetaData size. The default value
   * is `0`.
   */
  int metadataSize;
  /**
   * The alpha channel data output by using portrait segmentation algorithm. This data matches the
   * size of the video frame, with each pixel value ranging from [0,255], where 0 represents the
   * background and 255 represents the foreground (portrait).
   * By setting this parameter, you can render the video background into various effects, such as
   * transparent, solid color, image, video, etc.
   */
  uint8_t* alphaBuffer;
  /**
   * This parameter only applies to video data in BGRA or RGBA format. Whether to extract the alpha
   * channel data from the video frame and automatically fill it into `alphaBuffer`:
   * - `true：Extract and fill the alpha channel data.`
   * - `false`: (Default) Do not extract and fill the Alpha channel data.
   * @note
   * For video data in BGRA or RGBA format, you can set the Alpha channel data in either of the
   * following ways:
   * - Automatically by setting this parameter to `true`.
   * - Manually through the `alphaBuffer` parameter.
   */
  bool fillAlphaBuffer;
  /**
   * When the video frame contains alpha channel data, it represents the relative position of
   * `alphaBuffer` and the video frame. See `ALPHA_STITCH_MODE`.
   */
  ALPHA_STITCH_MODE alphaStitchMode;

  /**
   * This parameter only applies to video data in Windows Texture format. It represents a pointer to
   * an object of type` ID3D11Texture2D`, which is used by a video frame.
   */
  void *d3d11Texture2d;

  /**
   * This parameter only applies to video data in Windows Texture format. It represents an index of an
   * `ID3D11Texture2D` texture object used by the video frame in the `ID3D11Texture2D` array.
   */
  int textureSliceIndex;

  /**
   * metadata info used for hdr video data
   */
  Hdr10MetadataInfo hdr10MetadataInfo;

  /**
   * By default, the color space properties of video frames will apply the Full Range and BT.709
   * standard configurations.
   */
  ColorSpace colorSpace;
};

/**
 * @brief Configurations of the video frame.
 *
 * @details
 * Note that the buffer provides a pointer to a pointer. This interface cannot modify the pointer of
 * the buffer, but it can modify the content of the buffer.
 *
 */
struct VideoFrame {
  VideoFrame():
  type(VIDEO_PIXEL_DEFAULT),
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
  metadata_size(0),
  sharedContext(0),
  textureId(0),
  d3d11Texture2d(NULL),
  alphaBuffer(NULL),
  alphaStitchMode(NO_ALPHA_STITCH),
  pixelBuffer(NULL),
  metaInfo(NULL){
    memset(matrix, 0, sizeof(matrix));
  }
  /**
   * The pixel format. See `VIDEO_PIXEL_FORMAT`.
   */
  VIDEO_PIXEL_FORMAT type;
  /**
   * The width of the video, in pixels.
   */
  int width;
  /**
   * The height of the video, in pixels.
   */
  int height;
  /**
   * For YUV data, the line span of the Y buffer; for RGBA data, the total data length.
   * @note When dealing with video data, it is necessary to process the offset between each line of
   * pixel data based on this parameter, otherwise it may result in image distortion.
   */
  int yStride;
  /**
   * For YUV data, the line span of the U buffer; for RGBA data, the value is 0.
   * @note When dealing with video data, it is necessary to process the offset between each line of
   * pixel data based on this parameter, otherwise it may result in image distortion.
   */
  int uStride;
  /**
   * For YUV data, the line span of the V buffer; for RGBA data, the value is 0.
   * @note When dealing with video data, it is necessary to process the offset between each line of
   * pixel data based on this parameter, otherwise it may result in image distortion.
   */
  int vStride;
  /**
   * For YUV data, the pointer to the Y buffer; for RGBA data, the data buffer.
   */
  uint8_t* yBuffer;
  /**
   * For YUV data, the pointer to the U buffer; for RGBA data, the value is 0.
   */
  uint8_t* uBuffer;
  /**
   * For YUV data, the pointer to the V buffer; for RGBA data, the value is 0.
   */
  uint8_t* vBuffer;
  /**
   * The clockwise rotation of the video frame before rendering. Supported values include 0, 90, 180,
   * and 270 degrees.
   */
  int rotation;
  /**
   * The Unix timestamp (ms) when the video frame is rendered. This timestamp can be used to guide the
   * rendering of the video frame. This parameter is required.
   */
  int64_t renderTimeMs;
  /**
   * Reserved for future use.
   */
  int avsync_type;
  /**
   * This parameter only applies to video data in Texture format. The MetaData buffer. The default
   * value is `NULL`.
   */
  uint8_t* metadata_buffer;
  /**
   * This parameter only applies to video data in Texture format. The MetaData size. The default value
   * is `0`.
   */
  int metadata_size;
  /**
   * This parameter only applies to video data in Texture format. EGL Context.
   */
  void* sharedContext;
  /**
   * This parameter only applies to video data in Texture format. Texture ID.
   */
  int textureId;
  /**
   * This parameter only applies to video data in Windows Texture format. It represents a pointer to
   * an object of type` ID3D11Texture2D`, which is used by a video frame.
   */
  void* d3d11Texture2d;
  /**
   * This parameter only applies to video data in Texture format. Incoming 4 × 4 transformational
   * matrix. The typical value is a unit matrix.
   */
  float matrix[16];
  /**
   * The alpha channel data output by using portrait segmentation algorithm. This data matches the
   * size of the video frame, with each pixel value ranging from [0,255], where 0 represents the
   * background and 255 represents the foreground (portrait).
   * By setting this parameter, you can render the video background into various effects, such as
   * transparent, solid color, image, video, etc.
   * @note Make sure that `alphaBuffer` is exactly the same size as the video frame (width ×
   * height), otherwise it may cause the app to crash.
   */
  uint8_t* alphaBuffer;
  /**
   * When the video frame contains alpha channel data, it represents the relative position of
   * `alphaBuffer` and the video frame. See `ALPHA_STITCH_MODE`.
   */
  ALPHA_STITCH_MODE alphaStitchMode;
  /**
   *The type of CVPixelBufferRef, for iOS and macOS only.
   */
  void* pixelBuffer;
  /**
   * The meta information in the video frame. To use this parameter, contact `technical support`.
   */
  IVideoFrameMetaInfo* metaInfo;

  /**
   * metadata info used for hdr video data
   */
  Hdr10MetadataInfo hdr10MetadataInfo;

  /**
   * By default, the color space properties of video frames will apply the Full Range and BT.709
   * standard configurations.
   */
  ColorSpace colorSpace;
};

/**
 * The IVideoFrameObserver class.
 */
class IVideoFrameObserver {
 public:
  /**
   * @brief Occurs each time the player receives a video frame.
   *
   * @details
   * After registering the video frame observer, the callback occurs every time the player receives a
   * video frame, reporting the detailed information of the video frame.
   *
   * @param frame The video frame information. See VideoFrame.
   *
   */
  virtual void onFrame(const VideoFrame* frame) = 0;
  virtual ~IVideoFrameObserver() {}
  virtual bool isExternal() { return true; }
  /**
   * @brief Sets the format of the raw video data output by the SDK.
   *
   * @details
   * You need to register the callback when calling the `registerVideoFrameObserver` method. After you
   * successfully register the video frame observer, the SDK triggers this callback each time it
   * receives a video frame. You need to set your preferred video data in the return value of this
   * callback.
   *
   * @note
   * The default types of pixel format ( VIDEO_PIXEL_DEFAULT ) for the raw video are as follows:
   * - On the Android platform, the default video frame type may be I420Buffer or TextureBuffer. The
   * texture format of TextureBuffer type may be OES or RGB. If the returned video frame type is
   * VIDEO_PIXEL_DEFAULT when you call `getVideoFormatPreference`, you need to adapt to I420Buffer or
   * TextureBuffer when processing video data. The cases where the video frame type is fixed as
   * I420Buffer include but are not limited to:
   *   - Specific devices, such as: LG G5 SE (H848), Google Pixel 4a, Samsung Galaxy A7, or Xiaomi Mi
   * Max.
   *   - Image enhancement extension has been integrated and video noise reduction or low-light
   * enhancement function has been enabled.
   * - On iOS and macOS platforms, the default video frame type may be I420 or CVPixelBufferRef.
   * - On Windows platforms, the default video frame type is YUV420.
   *
   * @return
   * Sets the raw data format of the SDK output. See `VIDEO_PIXEL_FORMAT`.
   */
  virtual VIDEO_PIXEL_FORMAT getVideoFormatPreference() { return VIDEO_PIXEL_DEFAULT; }
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

/**
 * @brief The frame position of the video observer.
 */
enum VIDEO_MODULE_POSITION {
  /**
   * 1: The location of the locally collected video data after preprocessing corresponds to the
   * `onCaptureVideoFrame` callback. The observed video here has the effect of video pre-processing,
   * which can be verified by enabling image enhancement, virtual background, or watermark.
   */
  POSITION_POST_CAPTURER = 1 << 0,
  /**
   * 2: The pre-renderer position , which corresponds to the video data in the `onRenderVideoFrame`
   * callback.
   */
  POSITION_PRE_RENDERER = 1 << 1,
  /**
   * 4: The pre-encoder position , which corresponds to the video data in the `onPreEncodeVideoFrame`
   * callback. The observed video here has the effects of video pre-processing and encoding
   * pre-processing.
   * - To verify the pre-processing effects of the video, you can enable image enhancement, virtual
   * background, or watermark.
   * - To verify the pre-encoding processing effect, you can set a lower frame rate (for example, 5
   * fps).
   */
  POSITION_PRE_ENCODER = 1 << 2,
  /**
   * 8: The position after local video capture and before pre-processing. The observed video here does
   * not have pre-processing effects, which can be verified by enabling image enhancement, virtual
   * background, or watermarks.
   */
  POSITION_POST_CAPTURER_ORIGIN = 1 << 3,
};

}  // namespace base

/** Definition of contentinspect
 */
#define MAX_CONTENT_INSPECT_MODULE_COUNT 32
enum CONTENT_INSPECT_RESULT {
  CONTENT_INSPECT_NEUTRAL = 1,
  CONTENT_INSPECT_SEXY = 2,
  CONTENT_INSPECT_PORN = 3,
};

/**
 * @brief The type of video content moderation module.
 */
enum CONTENT_INSPECT_TYPE {
  /**
   * 0: (Default) This module has no actual function. Do not set `type` to this value.
   */
  CONTENT_INSPECT_INVALID = 0,
  /**
   * @deprecated
   * Content inspect type moderation
   */
  CONTENT_INSPECT_MODERATION __deprecated = 1,
  /**
   * 2: Video screenshot and upload via Agora self-developed extension. SDK takes screenshots of the
   * video stream in the channel and uploads them.
   */
  CONTENT_INSPECT_SUPERVISION = 2,
  /**
   * 3: Video screenshot and upload via extensions from Agora Extensions Marketplace. SDK uses video
   * moderation extensions from Agora Extensions Marketplace to take screenshots of the video stream
   * in the channel and uploads them.
   */
  CONTENT_INSPECT_IMAGE_MODERATION = 3
};

/**
 * @brief `ContentInspectModule` class, a structure used to configure the frequency of video
 * screenshot and upload.
 */
struct ContentInspectModule {
  /**
   * Types of functional module. See `CONTENT_INSPECT_TYPE`.
   */
  CONTENT_INSPECT_TYPE type;
  /**
   * The frequency (s) of video screenshot and upload. The value should be set as larger than 0. The
   * default value is 0, the SDK does not take screenshots. Agora recommends that you set the value as
   * 10; you can also adjust it according to your business needs.
   */
  unsigned int interval;
  /**
   * The position of the video observer. See `VIDEO_MODULE_POSITION`.
   */
  base::VIDEO_MODULE_POSITION position;
  ContentInspectModule() {
    type = CONTENT_INSPECT_INVALID;
    interval = 0;
    position = base::POSITION_PRE_ENCODER;
  }
};
/**
 * @brief Screenshot and upload configuration.
 */
struct ContentInspectConfig {
  /**
   * Additional information on the video content (maximum length: 1024 Bytes).
   * The SDK sends the screenshots and additional information on the video content to the Agora
   * server. Once the video screenshot and upload process is completed, the Agora server sends the
   * additional information and the callback notification to your server.
   */
  const char* extraInfo;
  /**
   * (Optional) Server configuration related to uploading video screenshots via extensions from Agora
   * Extensions Marketplace. This parameter only takes effect when `type` in `ContentInspectModule` is
   * set to `CONTENT_INSPECT_IMAGE_MODERATION`. If you want to use it, contact `technical support`.
   */
  const char* serverConfig;
  /**
   * Functional module. See `ContentInspectModule`.
   * A maximum of 32 `ContentInspectModule` instances can be configured, and the value range of
   * `MAX_CONTENT_INSPECT_MODULE_COUNT` is an integer in [1,32].
   * @note A function module can only be configured with one instance at most. Currently only the
   * video screenshot and upload function is supported.
   */
  ContentInspectModule modules[MAX_CONTENT_INSPECT_MODULE_COUNT];
  /**
   * The number of functional modules, that is,the number of configured `ContentInspectModule`
   * instances, must be the same as the number of instances configured in `modules`. The maximum
   * number is 32.
   */
  int moduleCount;
  ContentInspectConfig& operator=(const ContentInspectConfig& rth) {
    extraInfo = rth.extraInfo;
    serverConfig = rth.serverConfig;
    moduleCount = rth.moduleCount;
    memcpy(&modules, &rth.modules, MAX_CONTENT_INSPECT_MODULE_COUNT * sizeof(ContentInspectModule));
    return *this;
  }
  ContentInspectConfig() : extraInfo(NULL), serverConfig(NULL), moduleCount(0) {}
};
/**
 * @brief The snapshot configuration.
 */
struct SnapshotConfig {
  /**
   * The local path (including filename extensions) of the snapshot. For example:
   * - Windows: `C:\Users\<user_name>\AppData\Local\Agora\<process_name>\example.jpg`
   * - iOS:` /App Sandbox/Library/Caches/example.jpg`
   * - macOS: `～/Library/Logs/example.jpg`
   * - Android:` /storage/emulated/0/Android/data/<package name>/files/example.jpg`
   * @note Ensure that the path you specify exists and is writable.
   */
  const char* filePath;

  /**
   * The position of the snapshot video frame in the video pipeline. See `VIDEO_MODULE_POSITION`.
   */
  media::base::VIDEO_MODULE_POSITION position;
  SnapshotConfig() :filePath(NULL), position(media::base::POSITION_PRE_ENCODER) {}
};

/**
 * The audio frame observer.
 */
class IAudioPcmFrameSink {
 public:
  /**
   * @brief Occurs each time the player receives an audio frame.
   *
   * @details
   * After registering the audio frame observer, the callback occurs every time the player receives an
   * audio frame, reporting the detailed information of the audio frame.
   *
   * @param frame The audio frame information. See AudioPcmFrame.
   *
   */
  virtual void onFrame(agora::media::base::AudioPcmFrame* frame) = 0;
  virtual ~IAudioPcmFrameSink() {}
};

/**
 * The IAudioFrameObserverBase class.
 */
class IAudioFrameObserverBase {
 public:
  /**
   * @brief Audio frame type.
   */
  enum AUDIO_FRAME_TYPE {
    /**
     * 0: PCM 16
     */
    FRAME_TYPE_PCM16 = 0,
  };
  enum { MAX_HANDLE_TIME_CNT = 10 };
  /**
   * @brief Raw audio data.
   */
  struct AudioFrame {
    /**
     * The type of the audio frame. See `AUDIO_FRAME_TYPE`.
     */
    AUDIO_FRAME_TYPE type;
    /**
     * The number of samples per channel in the audio frame.
     */
    int samplesPerChannel;
    /**
     * The number of bytes per sample. For PCM, this parameter is generally set to 16 bits (2 bytes).
     */
    agora::rtc::BYTES_PER_SAMPLE bytesPerSample;
    /**
     * The number of audio channels (the data are interleaved if it is stereo).
     * - 1: Mono.
     * - 2: Stereo.
     */
    int channels;
    /**
     * The number of samples per channel in the audio frame.
     */
    int samplesPerSec;
    /**
     * The data buffer of the audio frame. When the audio frame uses a stereo channel, the data buffer
     * is interleaved.
     * The size of the data buffer is as follows: `buffer` = `samples` × `channels` × `bytesPerSample`.
     */
    void* buffer;
    /**
     * The timestamp (ms) of the external audio frame.
     * You can use this timestamp to restore the order of the captured audio frame, and synchronize
     * audio and video frames in video scenarios, including scenarios where external video sources are
     * used.
     */
    int64_t renderTimeMs;
    /**
     * Reserved for future use.
     */
    int avsync_type;
    /**
     * The pts timestamp of this audio frame.
     *
     * This timestamp is used to indicate the origin pts time of the frame, and sync with video
     * frame by the pts time stamp
     */
    int64_t presentationMs;
    /**
     * The number of the audio track.
     */
    int audioTrackNumber;
    /**
     * RTP timestamp of the first sample in the audio frame
     */
    uint32_t rtpTimestamp;

    AudioFrame()
        : type(FRAME_TYPE_PCM16),
          samplesPerChannel(0),
          bytesPerSample(rtc::TWO_BYTES_PER_SAMPLE),
          channels(0),
          samplesPerSec(0),
          buffer(NULL),
          renderTimeMs(0),
          avsync_type(0),
          presentationMs(0),
          audioTrackNumber(0),
          rtpTimestamp(0) {}
  };

  enum AUDIO_FRAME_POSITION {
    AUDIO_FRAME_POSITION_NONE = 0x0000,
    /** The position for observing the playback audio of all remote users after mixing
     */
    AUDIO_FRAME_POSITION_PLAYBACK = 0x0001,
    /** The position for observing the recorded audio of the local user
     */
    AUDIO_FRAME_POSITION_RECORD = 0x0002,
    /** The position for observing the mixed audio of the local user and all remote users
     */
    AUDIO_FRAME_POSITION_MIXED = 0x0004,
    /** The position for observing the audio of a single remote user before mixing
     */
    AUDIO_FRAME_POSITION_BEFORE_MIXING = 0x0008,
    /** The position for observing the ear monitoring audio of the local user
     */
    AUDIO_FRAME_POSITION_EAR_MONITORING = 0x0010,
  };

  /**
   * @brief Audio data format.
   *
   * @details
   * You can pass the `AudioParams` object in the following APIs to set the audio data format for the
   * corresponding callback:
   * - `getRecordAudioParams`: Sets the audio data format for the `onRecordAudioFrame` callback.
   * - `getPlaybackAudioParams`: Sets the audio data format for the `onPlaybackAudioFrame` callback.
   * - `getMixedAudioParams`: Sets the audio data format for the `onMixedAudioFrame` callback.
   * - `getEarMonitoringAudioParams`: Sets the audio data format for the `onEarMonitoringAudioFrame`
   * callback.
   *
   * @note
   * - The SDK calculates the sampling interval through the `samplesPerCall`, `sampleRate`, and
   * `channel` parameters in `AudioParams`, and triggers the `onRecordAudioFrame`,
   * `onPlaybackAudioFrame`, `onMixedAudioFrame`, and `onEarMonitoringAudioFrame` callbacks according
   * to the sampling interval.
   * - Sample interval (sec) = `samplePerCall` /( `sampleRate` × `channel` ).
   * - Ensure that the sample interval ≥ 0.01 (s).
   *
   */
  struct AudioParams {
    /**
     * The audio sample rate (Hz), which can be set as one of the following values:
     * - 8000.
     * - (Default) 16000.
     * - 32000.
     * - 44100
     * - 48000
     */
    int sample_rate;

    /**
     * The number of audio channels, which can be set as either of the following values:
     * - 1: (Default) Mono.
     * - 2: Stereo.
     */
    int channels;

    /**
     * The use mode of the audio data. See `RAW_AUDIO_FRAME_OP_MODE_TYPE`.
     */
    rtc::RAW_AUDIO_FRAME_OP_MODE_TYPE mode;

    /**
     * The number of samples, such as 1024 for the media push.
     */
    int samples_per_call;

    AudioParams()
        : sample_rate(0),
          channels(0),
          mode(rtc::RAW_AUDIO_FRAME_OP_MODE_READ_ONLY),
          samples_per_call(0) {}
    AudioParams(int samplerate, int channel, rtc::RAW_AUDIO_FRAME_OP_MODE_TYPE type,
                int samplesPerCall)
        : sample_rate(samplerate),
          channels(channel),
          mode(type),
          samples_per_call(samplesPerCall) {}
  };

 public:
  virtual ~IAudioFrameObserverBase() {}

  /**
   * @brief Gets the captured audio frame.
   *
   * @details
   * To ensure that the format of the cpatured audio frame is as expected, you can choose one of the
   * following two methods to set the audio data format:
   * - Method 1: After calling `setRecordingAudioFrameParameters` to set the audio data format and
   * `registerAudioFrameObserver` to register the audio frame observer object, the SDK calculates the
   * sampling interval according to the parameters set in the methods, and triggers the
   * `onRecordAudioFrame` callback according to the sampling interval.
   * - Method 2: After calling `registerAudioFrameObserver` to register the audio frame observer
   * object, set the audio data format in the return value of the `getObservedAudioFramePosition`
   * callback. The SDK then calculates the sampling interval according to the return value of the
   * `getRecordAudioParams` callback, and triggers the `onRecordAudioFrame` callback according to the
   * sampling interval.
   *
   * @note The priority of method 1 is higher than that of method 2. If method 1 is used to set the
   * audio data format, the setting of method 2 is invalid.
   *
   * @param audioFrame The raw audio data. See `AudioFrame`.
   * @param channelId The channel ID.
   *
   * @return
   * Without practical meaning.
   */
  virtual bool onRecordAudioFrame(const char* channelId, AudioFrame& audioFrame) = 0;
  /**
   * @brief Gets the raw audio frame for playback.
   *
   * @details
   * To ensure that the data format of audio frame for playback is as expected, Agora recommends that
   * you choose one of the following two methods to set the audio data format:
   * - Method 1: After calling `setPlaybackAudioFrameParameters` to set the audio data format and
   * `registerAudioFrameObserver` to register the audio frame observer object, the SDK calculates the
   * sampling interval according to the parameters set in the methods, and triggers the
   * `onPlaybackAudioFrame` callback according to the sampling interval.
   * - Method 2: After calling `registerAudioFrameObserver` to register the audio frame observer
   * object, set the audio data format in the return value of the `getObservedAudioFramePosition`
   * callback. The SDK then calculates the sampling interval according to the return value of the
   * `getPlaybackAudioParams` callback, and triggers the `onPlaybackAudioFrame` callback according to
   * the sampling interval.
   *
   * @note The priority of method 1 is higher than that of method 2. If method 1 is used to set the
   * audio data format, the setting of method 2 is invalid.
   *
   * @param audioFrame The raw audio data. See `AudioFrame`.
   * @param channelId The channel ID.
   *
   * @return
   * Without practical meaning.
   */
  virtual bool onPlaybackAudioFrame(const char* channelId, AudioFrame& audioFrame) = 0;
  /**
   * @brief Retrieves the mixed captured and playback audio frame.
   *
   * @details
   * To ensure that the data format of mixed captured and playback audio frame meets the expectations,
   * Agora recommends that you choose one of the following two ways to set the data format:
   * - Method 1: After calling `setMixedAudioFrameParameters` to set the audio data format and
   * `registerAudioFrameObserver` to register the audio frame observer object, the SDK calculates the
   * sampling interval according to the parameters set in the methods, and triggers the
   * `onMixedAudioFrame` callback according to the sampling interval.
   * - Method 2: After calling `registerAudioFrameObserver` to register the audio frame observer
   * object, set the audio data format in the return value of the `getObservedAudioFramePosition`
   * callback. The SDK then calculates the sampling interval according to the return value of the
   * `getMixedAudioParams` callback, and triggers the `onMixedAudioFrame` callback according to the
   * sampling interval.
   *
   * @note The priority of method 1 is higher than that of method 2. If method 1 is used to set the
   * audio data format, the setting of method 2 is invalid.
   *
   * @param audioFrame The raw audio data. See `AudioFrame`.
   * @param channelId The channel ID.
   *
   * @return
   * Without practical meaning.
   */
  virtual bool onMixedAudioFrame(const char* channelId, AudioFrame& audioFrame) = 0;
  /**
   * @brief Gets the in-ear monitoring audio frame.
   *
   * @details
   * In order to ensure that the obtained in-ear audio data meets the expectations, Agora recommends
   * that you choose one of the following two methods to set the in-ear monitoring-ear audio data
   * format:
   * - Method 1: After calling `setEarMonitoringAudioFrameParameters` to set the audio data format and
   * `registerAudioFrameObserver` to register the audio frame observer object, the SDK calculates the
   * sampling interval according to the parameters set in the methods, and triggers the
   * `onEarMonitoringAudioFrame` callback according to the sampling interval.
   * - Method 2: After calling `registerAudioFrameObserver` to register the audio frame observer
   * object, set the audio data format in the return value of the `getObservedAudioFramePosition`
   * callback. The SDK then calculates the sampling interval according to the return value of the
   * `getEarMonitoringAudioParams` callback, and triggers the `onEarMonitoringAudioFrame` callback
   * according to the sampling interval.
   *
   * @note The priority of method 1 is higher than that of method 2. If method 1 is used to set the
   * audio data format, the setting of method 2 is invalid.
   *
   * @param audioFrame The raw audio data. See `AudioFrame`.
   *
   * @return
   * Without practical meaning.
   */
  virtual bool onEarMonitoringAudioFrame(AudioFrame& audioFrame) = 0;
  /**
   * Occurs when the before-mixing playback audio frame is received.
   * @param channelId The channel name
   * @param userId ID of the remote user.
   * @param audioFrame The reference to the audio frame: AudioFrame.
   * @return
   * - true: The before-mixing playback audio frame is valid and is encoded and sent.
   * - false: The before-mixing playback audio frame is invalid and is not encoded or sent.
   */
  virtual bool onPlaybackAudioFrameBeforeMixing(const char* channelId, base::user_id_t userId,
                                                AudioFrame& audioFrame) {
    (void)channelId;
    (void)userId;
    (void)audioFrame;
    return true;
  }

  /**
   * @brief Sets the frame position for the video observer.
   *
   * @details
   * After successfully registering the audio data observer, the SDK uses this callback for each
   * specific audio frame processing node to determine whether to trigger the following callbacks:
   * - `onRecordAudioFrame`
   * - `onPlaybackAudioFrame`
   * - `onPlaybackAudioFrameBeforeMixing`
   * - `onMixedAudioFrame`
   * - `onEarMonitoringAudioFrame`
   * You can set one or more positions you need to observe by modifying the return value of
   * `getObservedAudioFramePosition` based on your scenario requirements:
   * When the annotation observes multiple locations, the | (or operator) is required. To conserve
   * system resources, you can reduce the number of frame positions that you want to observe.
   *
   * @return
   * a bitmask that sets the observation position, with the following values:
   * - AUDIO_FRAME_POSITION_PLAYBACK (0x0001): This position can observe the playback audio mixed by
   * all remote users, corresponding to the `onPlaybackAudioFrame` callback.
   * - AUDIO_FRAME_POSITION_RECORD (0x0002): This position can observe the collected local user's
   * audio, corresponding to the `onRecordAudioFrame` callback.
   * - AUDIO_FRAME_POSITION_MIXED (0x0004): This position can observe the playback audio mixed by the
   * loacl user and all remote users, corresponding to the `onMixedAudioFrame` callback.
   * - AUDIO_FRAME_POSITION_BEFORE_MIXING (0x0008): This position can observe the audio of a single
   * remote user before mixing, corresponding to the `onPlaybackAudioFrameBeforeMixing` callback.
   * - AUDIO_FRAME_POSITION_EAR_MONITORING (0x0010): This position can observe the in-ear monitoring
   * audio of the local user, corresponding to the `onEarMonitoringAudioFrame` callback.
   */
  virtual int getObservedAudioFramePosition() = 0;

  /**
   * @brief Sets the audio format for the `onPlaybackAudioFrame` callback.
   *
   * @details
   * You need to register the callback when calling the `registerAudioFrameObserver` method. After you
   * successfully register the audio observer, the SDK triggers this callback, and you can set the
   * audio format in the return value of this callback.
   *
   * @note
   * The SDK triggers the `onPlaybackAudioFrame` callback with the `AudioParams` calculated sampling
   * interval you set in the return value. The calculation formula is Sample interval (sec) =
   * `samplePerCall` /( `sampleRate` × `channel` ).
   * Ensure that the sample interval ≥ 0.01 (s).
   *
   * @return
   * The audio data for playback, see `AudioParams`.
   */
  virtual AudioParams getPlaybackAudioParams() = 0;

  /**
   * @brief Sets the audio format for the `onRecordAudioFrame` callback.
   *
   * @details
   * You need to register the callback when calling the `registerAudioFrameObserver` method. After you
   * successfully register the audio observer, the SDK triggers this callback, and you can set the
   * audio format in the return value of this callback.
   *
   * @note
   * The SDK triggers the `onRecordAudioFrame` callback with the `AudioParams` calculated sampling
   * interval you set in the return value. The calculation formula is Sample interval (sec) =
   * `samplePerCall` /( `sampleRate` × `channel` ).
   * Ensure that the sample interval ≥ 0.01 (s).
   *
   * @return
   * The captured audio data, see `AudioParams`.
   */
  virtual AudioParams getRecordAudioParams() = 0;

  /**
   * @brief Sets the audio format for the `onMixedAudioFrame` callback.
   *
   * @details
   * You need to register the callback when calling the `registerAudioFrameObserver` method. After you
   * successfully register the audio observer, the SDK triggers this callback, and you can set the
   * audio format in the return value of this callback.
   *
   * @note
   * The SDK triggers the `onMixedAudioFrame` callback with the `AudioParams` calculated sampling
   * interval you set in the return value. The calculation formula is Sample interval (sec) =
   * `samplePerCall` /( `sampleRate` × `channel` ).
   * Ensure that the sample interval ≥ 0.01 (s).
   *
   * @return
   * The mixed captured and playback audio data. See `AudioParams`.
   */
  virtual AudioParams getMixedAudioParams() = 0;

  /**
   * @brief Sets the audio format for the `onEarMonitoringAudioFrame` callback.
   *
   * @details
   * You need to register the callback when calling the `registerAudioFrameObserver` method. After you
   * successfully register the audio observer, the SDK triggers this callback, and you can set the
   * audio format in the return value of this callback.
   *
   * @note
   * The SDK triggers the `onEarMonitoringAudioFrame` callback with the `AudioParams` calculated
   * sampling interval you set in the return value. The calculation formula is `Sample` interval (
   * `sec` ) = `samplePerCall` /( `sampleRate` × `channel` ).
   * Ensure that the sample interval ≥ 0.01 (s).
   *
   * @return
   * The audio data of in-ear monitoring, see `AudioParams`.
   */
  virtual AudioParams getEarMonitoringAudioParams() = 0;
};

/**
 * The IAudioFrameObserver class.
 */
class IAudioFrameObserver : public IAudioFrameObserverBase {
 public:
  using IAudioFrameObserverBase::onPlaybackAudioFrameBeforeMixing;
  /**
   * @brief Retrieves the audio frame before mixing of subscribed remote users.
   *
   * @param channelId The channel ID.
   * @param uid The ID of subscribed remote users.
   * @param audioFrame The raw audio data. See `AudioFrame`.
   *
   * @return
   * Without practical meaning.
   */
  virtual bool onPlaybackAudioFrameBeforeMixing(const char* channelId, rtc::uid_t uid,
                                                AudioFrame& audioFrame) = 0;
};

/**
 * @brief The audio spectrum data.
 */
struct AudioSpectrumData {
  /**
   * The audio spectrum data. Agora divides the audio frequency into 256 frequency domains, and
   * reports the energy value of each frequency domain through this parameter. The value range of each
   * energy type is [-300, 1] and the unit is dBFS.
   */
  const float* audioSpectrumData;
  /**
   * The audio spectrum data length is 256.
   */
  int dataLength;

  AudioSpectrumData() : audioSpectrumData(NULL), dataLength(0) {}
  AudioSpectrumData(const float* data, int length) : audioSpectrumData(data), dataLength(length) {}
};

/**
 * @brief Audio spectrum information of the remote user.
 */
struct UserAudioSpectrumInfo {
  /**
   * The user ID of the remote user.
   */
  agora::rtc::uid_t uid;
  /**
   * Audio spectrum information of the remote user. See `AudioSpectrumData`.
   */
  struct AudioSpectrumData spectrumData;

  UserAudioSpectrumInfo() : uid(0) {}

  UserAudioSpectrumInfo(agora::rtc::uid_t uid, const float* data, int length)
      : uid(uid), spectrumData(data, length) {}
};

/**
 * The IAudioSpectrumObserver class.
 */
class IAudioSpectrumObserver {
 public:
  virtual ~IAudioSpectrumObserver() {}

  /**
   * @brief Gets the statistics of a local audio spectrum.
   *
   * @details
   * After successfully calling `registerAudioSpectrumObserver` to implement the
   * `onLocalAudioSpectrum` callback in `IAudioSpectrumObserver` and calling
   * `enableAudioSpectrumMonitor` to enable audio spectrum monitoring, the SDK triggers this callback
   * as the time interval you set to report the received remote audio data spectrum before encoding.
   *
   * @param data The audio spectrum data of the local user. See `AudioSpectrumData`.
   *
   * @return
   * Whether the spectrum data is received:
   * - `true`: Spectrum data is received.
   * - `false`: No spectrum data is received.
   */
  virtual bool onLocalAudioSpectrum(const AudioSpectrumData& data) = 0;
  /**
   * @brief Gets the remote audio spectrum.
   *
   * @details
   * After successfully calling `registerAudioSpectrumObserver` to implement the
   * `onRemoteAudioSpectrum` callback in the `IAudioSpectrumObserver` and calling
   * `enableAudioSpectrumMonitor` to enable audio spectrum monitoring, the SDK will trigger the
   * callback as the time interval you set to report the received remote audio data spectrum.
   *
   * @param spectrums The audio spectrum information of the remote user. See `UserAudioSpectrumInfo`.
   * The number of arrays is the number of remote users monitored by the SDK. If the array is null, it
   * means that no audio spectrum of remote users is detected.
   * @param spectrumNumber The number of remote users.
   *
   * @return
   * Whether the spectrum data is received:
   * - `true`: Spectrum data is received.
   * - `false`: No spectrum data is received.
   */
  virtual bool onRemoteAudioSpectrum(const UserAudioSpectrumInfo* spectrums,
                                     unsigned int spectrumNumber) = 0;
};

/**
 * The IVideoEncodedFrameObserver class.
 */
class IVideoEncodedFrameObserver {
 public:
  /**
   * @brief Reports that the receiver has received the to-be-decoded video frame sent by the remote
   * end.
   *
   * @details
   * If you call the `setRemoteVideoSubscriptionOptions` method and set `encodedFrameOnly` to `true`,
   * the SDK triggers this callback locally to report the received encoded video frame information.
   *
   * @since 4.6.0
   * @param channelId The channel name.
   * @param uid The user ID of the remote user.
   * @param imageBuffer The encoded video image buffer.
   * @param length The data length of the video image.
   * @param videoEncodedFrameInfo For the information of the encoded video frame, see
   * `EncodedVideoFrameInfo`.
   *
   * @return
   * Without practical meaning.
   */
  virtual bool onEncodedVideoFrameReceived(
      const char* channelId, rtc::uid_t uid, const uint8_t* imageBuffer, size_t length,
      const rtc::EncodedVideoFrameInfo& videoEncodedFrameInfo) = 0;

  virtual ~IVideoEncodedFrameObserver() {}
};

/**
 * The IVideoFrameObserver class.
 */
class IVideoFrameObserver {
 public:
  typedef media::base::VideoFrame VideoFrame;
  /**
   * @brief The process mode of the video frame:
   */
  enum VIDEO_FRAME_PROCESS_MODE {
    /**
     * Read-only mode.
     * In this mode, you do not modify the video frame. The video frame observer is a renderer.
     */
    PROCESS_MODE_READ_ONLY,  // Observer works as a pure renderer and will not modify the original
                             // frame.
    /**
     * Read and write mode.
     * In this mode, you modify the video frame. The video frame observer is a video filter.
     */
    PROCESS_MODE_READ_WRITE,  // Observer works as a filter that will process the video frame and
                              // affect the following frame processing in SDK.
  };

 public:
  virtual ~IVideoFrameObserver() {}

  /**
   * @brief Occurs each time the SDK receives a video frame captured by local devices.
   *
   * @details
   * You can get raw video data collected by the local device through this callback and preprocess it
   * as needed. Once the preprocessing is complete, you can directly modify `videoFrame` in this
   * callback, and set the return value to `true` to send the modified video data to the SDK.
   * If you need to send the preprocessed data to the SDK, you need to call `getVideoFrameProcessMode`
   * first to set the video processing mode to read and write mode ( PROCESS_MODE_READ_WRITE ).
   * Applicable scenarios: - Preprocess the locally collected video data before it is processed by the
   * SDK. For example, get video data through this callback and process it with filters, watermarks,
   * cropping, rotation, etc.
   * - Get information about the locally collected video data before it is processed by the SDK. For
   * example, the original width, height, frame rate of the video frame, etc.
   * Call timing: After the successful registration of the video data observer, each time the SDK
   * captures a video frame.
   *
   * @note
   * - If the video data type you get is RGBA, the SDK does not support processing the data of the
   * alpha channel.
   * - It is recommended that you ensure the modified parameters in `videoFrame` are consistent with
   * the actual situation of the video frames in the video frame buffer. Otherwise, it may cause
   * unexpected rotation, distortion, and other issues in the local preview and remote video display.
   * The default video format that you get from this callback is YUV420. If you need other formats,
   * you can set the expected data format in the getVideoFormatPreference callback.
   *
   * @param sourceType Video source types, including cameras, screens, or media player. See
   * `VIDEO_SOURCE_TYPE`.
   * @param videoFrame The video frame. See `VideoFrame`.Note: The default value of the video frame
   * data format obtained through this callback is as follows:
   * - Android: I420 or RGB (GLES20.GL_TEXTURE_2D)
   * - iOS: I420 or CVPixelBufferRef
   * - macOS: I420 or CVPixelBufferRef
   * - Windows: YUV420
   *
   * @return
   * - When the video processing mode is `PROCESS_MODE_READ_ONLY`:
   *   - `true`: Reserved for future use.
   *   - `false`: Reserved for future use.
   * - When the video processing mode is `PROCESS_MODE_READ_WRITE`:
   *   - `true`: Sets the SDK to receive the video frame.
   *   - `false`: Sets the SDK to discard the video frame.
   */
  virtual bool onCaptureVideoFrame(agora::rtc::VIDEO_SOURCE_TYPE sourceType,
                                   VideoFrame& videoFrame) = 0;

  /**
   * @brief Occurs each time the SDK receives a video frame before encoding.
   *
   * @details
   * After you successfully register the video frame observer, the SDK triggers this callback each
   * time it receives a video frame. In this callback, you can get the video data before encoding and
   * then process the data according to your particular scenarios.
   * After processing, you can send the processed video data back to the SDK in this callback.
   *
   * @note
   * - If you need to send the preprocessed data to the SDK, you need to call
   * `getVideoFrameProcessMode` first to set the video processing mode to read and write mode (
   * PROCESS_MODE_READ_WRITE ).
   * - To get the video data captured from the second screen before encoding, you need to set
   * `POSITION_PRE_ENCODER` (1 << 2) as a frame position through `getObservedFramePosition`.
   * - The video data that this callback gets has been preprocessed, with its content cropped and
   * rotated, and the image enhanced.
   * - It is recommended that you ensure the modified parameters in `videoFrame` are consistent with
   * the actual situation of the video frames in the video frame buffer. Otherwise, it may cause
   * unexpected rotation, distortion, and other issues in the local preview and remote video display.
   *
   * @param sourceType The type of the video source. See `VIDEO_SOURCE_TYPE`.
   * @param videoFrame The video frame. See `VideoFrame`.Note: The default value of the video frame
   * data format obtained through this callback is as follows:
   * - Android: I420 or RGB (GLES20.GL_TEXTURE_2D)
   * - iOS: I420 or CVPixelBufferRef
   * - macOS: I420 or CVPixelBufferRef
   * - Windows: YUV420
   *
   * @return
   * - When the video processing mode is `PROCESS_MODE_READ_ONLY`:
   *   - `true`: Reserved for future use.
   *   - `false`: Reserved for future use.
   * - When the video processing mode is `PROCESS_MODE_READ_WRITE`:
   *   - `true`: Sets the SDK to receive the video frame.
   *   - `false`: Sets the SDK to discard the video frame.
   */
  virtual bool onPreEncodeVideoFrame(agora::rtc::VIDEO_SOURCE_TYPE sourceType,
                                     VideoFrame& videoFrame) = 0;

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
   * @note
   * - This callback will not be affected by the return values of \ref getVideoFrameProcessMode
   * "getVideoFrameProcessMode", \ref getRotationApplied "getRotationApplied", \ref getMirrorApplied
   * "getMirrorApplied", \ref getObservedFramePosition "getObservedFramePosition".
   * - On Android, this callback is not affected by the return value of \ref
   * getVideoFormatPreference "getVideoFormatPreference"
   *
   * @param videoFrame A pointer to the video frame: VideoFrame
   * @param mediaPlayerId ID of the mediaPlayer.
   * @return Determines whether to ignore the current video frame if the pre-processing fails:
   * - true: Do not ignore.
   * - false: Ignore, in which case this method does not sent the current video frame to the SDK.
   */
  virtual bool onMediaPlayerVideoFrame(VideoFrame& videoFrame, int mediaPlayerId) = 0;

  /**
   * @brief Occurs each time the SDK receives a video frame sent by the remote user.
   *
   * @details
   * After you successfully register the video frame observer, the SDK triggers this callback each
   * time it receives a video frame. In this callback, you can get the video data sent from the remote
   * end before rendering, and then process it according to the particular scenarios.
   * The default video format that you get from this callback is YUV420. If you need other formats,
   * you can set the expected data format in the `getVideoFormatPreference` callback.
   *
   * @note
   * - If you need to send the preprocessed data to the SDK, you need to call
   * `getVideoFrameProcessMode` first to set the video processing mode to read and write mode (
   * PROCESS_MODE_READ_WRITE ).
   * - If the video data type you get is RGBA, the SDK does not support processing the data of the
   * alpha channel.
   * - It is recommended that you ensure the modified parameters in `videoFrame` are consistent with
   * the actual situation of the video frames in the video frame buffer. Otherwise, it may cause
   * unexpected rotation, distortion, and other issues in the local preview and remote video display.
   *
   * @param remoteUid The user ID of the remote user who sends the current video frame.
   * @param videoFrame The video frame. See `VideoFrame`.Note: The default value of the video frame
   * data format obtained through this callback is as follows:
   * - Android: I420 or RGB (GLES20.GL_TEXTURE_2D)
   * - iOS: I420 or CVPixelBufferRef
   * - macOS: I420 or CVPixelBufferRef
   * - Windows: YUV420
   * @param channelId The channel ID.
   *
   * @return
   * - When the video processing mode is `PROCESS_MODE_READ_ONLY`:
   *   - `true`: Reserved for future use.
   *   - `false`: Reserved for future use.
   * - When the video processing mode is `PROCESS_MODE_READ_WRITE`:
   *   - `true`: Sets the SDK to receive the video frame.
   *   - `false`: Sets the SDK to discard the video frame.
   */
  virtual bool onRenderVideoFrame(const char* channelId, rtc::uid_t remoteUid,
                                  VideoFrame& videoFrame) = 0;

  virtual bool onTranscodedVideoFrame(VideoFrame& videoFrame) = 0;

  /**
   * @brief Occurs each time the SDK receives a video frame and prompts you to set the process mode of
   * the video frame.
   *
   * @details
   * After you successfully register the video frame observer, the SDK triggers this callback each
   * time it receives a video frame. You need to set your preferred process mode in the return value
   * of this callback.
   *
   * @return
   * See `VIDEO_FRAME_PROCESS_MODE`.
   */
  virtual VIDEO_FRAME_PROCESS_MODE getVideoFrameProcessMode() { return PROCESS_MODE_READ_ONLY; }

  /**
   * Sets the format of the raw video data output by the SDK.
   *
   * If you want to get raw video data in a color encoding format other than YUV 420, register this
   * callback when calling `registerVideoFrameObserver`. After you successfully register the video
   * frame observer, the SDK triggers this callback each time it receives a video frame. You need to
   * set your preferred video data in the return value of this callback.
   *
   * @note If you want the video captured by the sender to be the original format, set the original
   * video data format to VIDEO_PIXEL_DEFAULT in the return value. On different platforms, the
   * original video pixel format is also different, for the actual video pixel format, see
   * `VideoFrame`.
   *
   * @return Sets the video format. See VIDEO_PIXEL_FORMAT.
   */
  virtual base::VIDEO_PIXEL_FORMAT getVideoFormatPreference() { return base::VIDEO_PIXEL_DEFAULT; }

  /**
   * @brief Occurs each time the SDK receives a video frame, and prompts you whether to rotate the
   * captured video.
   *
   * @details
   * If you want to rotate the captured video according to the `rotation` member in the `VideoFrame`
   * class, ensure that you register this callback when calling `registerVideoFrameObserver`. After
   * you successfully register the video frame observer, the SDK triggers this callback each time it
   * receives a video frame. You need to set whether to rotate the video frame in the return value of
   * this callback.
   *
   * @note
   * - On the Android platform, the supported video data formats for this callback are: I420, RGBA,
   * and Texture.
   * - On the Windows platform, the supported video data formats for this callback are: I420, RGBA,
   * and TextureBuffer.
   * - On the iOS platform, the supported video data formats for this callback are: I420, RGBA, and
   * CVPixelBuffer.
   * - On the macOS platform, the supported video data formats for this callback are: I420 and RGBA.
   *
   * @return
   * Sets whether to rotate the captured video:
   * - `true`: Rotate the captured video.
   * - `false`: (Default) Do not rotate the captured video.
   */
  virtual bool getRotationApplied() { return false; }

  /**
   * @brief Occurs each time the SDK receives a video frame and prompts you whether or not to mirror
   * the captured video.
   *
   * @details
   * If the video data you want to obtain is a mirror image of the original video, you need to
   * register this callback when calling `registerVideoFrameObserver`. After you successfully register
   * the video frame observer, the SDK triggers this callback each time it receives a video frame. You
   * need to set whether or not to mirror the video frame in the return value of this callback.
   *
   * @note
   * - On the Android platform, the supported video data formats for this callback are: I420, RGBA,
   * and Texture.
   * - On the Windows platform, the supported video data formats for this callback are: I420, RGBA,
   * and TextureBuffer.
   * - On the iOS platform, the supported video data formats for this callback are: I420, RGBA, and
   * CVPixelBuffer.
   * - On the macOS platform, the supported video data formats for this callback are: I420 and RGBA.
   * - Both this method and the `setVideoEncoderConfiguration` method support setting the mirroring
   * effect. Agora recommends that you only choose one method to set it up. Using both methods at the
   * same time causes the mirroring effect to overlap, and the mirroring settings fail.
   *
   * @return
   * Sets whether or not to mirror the captured video:
   * - `true`: Mirror the captured video.
   * - `false`: (Default) Do not mirror the captured video.
   */
  virtual bool getMirrorApplied() { return false; }

  /**
   * @brief Sets the frame position for the video observer.
   *
   * @details
   * After successfully registering the video data observer, the SDK uses this callback to determine
   * whether to trigger `onCaptureVideoFrame`, `onRenderVideoFrame` and `onPreEncodeVideoFrame`
   * callback at each specific video frame processing position, so that you can observe the locally
   * collected video data, the video data sent by the remote end, and the video data before encoding.
   * You can set one or more positions you need to observe by modifying the return value according to
   * your scenario:
   * - `POSITION_POST_CAPTURER` (1 << 0): The position after capturing the video data, which
   * corresponds to the `onCaptureVideoFrame` callback.
   * - `POSITION_PRE_RENDERER` (1 << 1): The position of the received remote video data before
   * rendering, which corresponds to the `onRenderVideoFrame` callback.
   * - `POSITION_PRE_ENCODER` (1 << 2): The position before encoding the video data, which corresponds
   * to the `onPreEncodeVideoFrame` callback.
   *
   * @note
   * - Use '|' (the OR operator) to observe multiple frame positions.
   * - This callback observes `POSITION_POST_CAPTURER` (1 << 0) and `POSITION_PRE_RENDERER` (1 << 1)
   * by default.
   * - To conserve system resources, you can reduce the number of frame positions that you want to
   * observe.
   * - When the video processing mode is `PROCESS_MODE_READ_WRITE` and the observation position is set
   * to `POSITION_PRE_ENCODER` | `POSITION_POST_CAPTURER`, the `getMirrorApplied` does not take
   * effect; you need to modify the video processing mode or the position of the observer.
   *
   * @return
   * A bit mask that controls the frame position of the video observer. See `VIDEO_MODULE_POSITION`.
   */
  virtual uint32_t getObservedFramePosition() {
    return base::POSITION_POST_CAPTURER | base::POSITION_PRE_RENDERER;
  }

  /**
   * Indicate if the observer is for internal use.
   * Note: Never override this function
   * @return
   * - true: the observer is for external use
   * - false: the observer is for internal use
   */
  virtual bool isExternal() { return true; }
};

/**
 * @brief The external video frame encoding type.
 */
enum EXTERNAL_VIDEO_SOURCE_TYPE {
  /**
   * 0: The video frame is not encoded.
   */
  VIDEO_FRAME = 0,
  /**
   * 1: The video frame is encoded.
   */
  ENCODED_VIDEO_FRAME,
};

/**
 * @brief Format of the recording file.
 *
 * @since v3.5.2
 */
enum MediaRecorderContainerFormat {
  /**
   * 1: (Default) MP4.
   */
  FORMAT_MP4 = 1,
};
/**
 * @brief The recording content.
 *
 * @since v3.5.2
 */
enum MediaRecorderStreamType {
  /**
   * Only audio.
   */
  STREAM_TYPE_AUDIO = 0x01,
  /**
   * Only video.
   */
  STREAM_TYPE_VIDEO = 0x02,
  /**
   * (Default) Audio and video.
   */
  STREAM_TYPE_BOTH = STREAM_TYPE_AUDIO | STREAM_TYPE_VIDEO,
};
/**
 * @brief The current recording state.
 *
 * @since v3.5.2
 */
enum RecorderState {
  /**
   * -1: An error occurs during the recording. See `RecorderReasonCode` for the reason.
   */
  RECORDER_STATE_ERROR = -1,
  /**
   * 2: The audio and video recording starts.
   */
  RECORDER_STATE_START = 2,
  /**
   * 3: The audio and video recording stops.
   */
  RECORDER_STATE_STOP = 3,
};
/**
 * @brief The reason for the state change.
 *
 * @since v3.5.2
 */
enum RecorderReasonCode {
  /**
   * 0: No error.
   */
  RECORDER_REASON_NONE = 0,
  /**
   * 1: The SDK fails to write the recorded data to a file.
   */
  RECORDER_REASON_WRITE_FAILED = 1,
  /**
   * 2: The SDK does not detect any audio and video streams, or audio and video streams are
   * interrupted for more than five seconds during recording.
   */
  RECORDER_REASON_NO_STREAM = 2,
  /**
   * 3: The recording duration exceeds the upper limit.
   */
  RECORDER_REASON_OVER_MAX_DURATION = 3,
  /**
   * 4: The recording configuration changes.
   */
  RECORDER_REASON_CONFIG_CHANGED = 4,
};
/**
 * @brief Configuration for audio and video stream recording.
 *
 * @since v3.5.2
 */
struct MediaRecorderConfiguration {
  /**
   * The absolute path where the recording file will be saved locally, including the file name and
   * format. For example:
   * - Windows: `C:\Users\<user_name>\AppData\Local\Agora\<process_name>\example.mp4`
   * - iOS: `/App Sandbox/Library/Caches/example.mp4`
   * - macOS: `/Library/Logs/example.mp4`
   * - Android: `/storage/emulated/0/Android/data/<package name>/files/example.mp4`
   * @note Make sure the specified path exists and is writable.
   */
  const char* storagePath;
  /**
   * The format of the recording file. See `MediaRecorderContainerFormat`.
   */
  MediaRecorderContainerFormat containerFormat;
  /**
   * The content to record. See `MediaRecorderStreamType`.
   */
  MediaRecorderStreamType streamType;
  /**
   * Maximum recording duration in milliseconds. Default is 120000.
   */
  int maxDurationMs;
  /**
   * Interval for recording information updates, in milliseconds. The valid range is [1000,10000]. The
   * SDK triggers the `onRecorderInfoUpdated` callback based on this value to report updated recording
   * information.
   */
  int recorderInfoUpdateInterval;
  /**
   * Width (px) of the recorded video. The maximum value for width × height must not exceed 3840 ×
   * 2160.
   * This parameter is required only when calling `createMediaRecorder` and setting `type` in
   * `RecorderStreamInfo` to PREVIEW.
   */
  int width;
  /**
   * Height (px) of the recorded video. The maximum value for width × height must not exceed 3840 ×
   * 2160.
   * This parameter is required only when calling `createMediaRecorder` and setting `type` in
   * `RecorderStreamInfo` to PREVIEW.
   */
  int height;
  /**
   * Frame rate of the recorded video. The maximum is 30. For example: 5, 10, 15, 24, 30.
   * This parameter is required only when calling `createMediaRecorder` and setting `type` in
   * `RecorderStreamInfo` to PREVIEW.
   */
  int fps;
  /**
   * Sample rate (Hz) of the recorded audio. Supported values: 16000, 32000, 44100, or 48000.
   * This parameter is required only when calling `createMediaRecorder` and setting `type` in
   * `RecorderStreamInfo` to PREVIEW.
   */
  int sample_rate;
  /**
   * Number of audio channels to record:
   * - 1: Mono
   * - 2: Stereo
   * This parameter is required only when calling `createMediaRecorder` and setting `type` in
   * `RecorderStreamInfo` to PREVIEW.
   */
  int channel_num;
  /**
   * Type of video source to record. See `VIDEO_SOURCE_TYPE`.
   * This parameter is required only when calling `createMediaRecorder` and setting `type` in
   * `RecorderStreamInfo` to PREVIEW.
   */
  agora::rtc::VIDEO_SOURCE_TYPE videoSourceType;

  MediaRecorderConfiguration()
      : storagePath(NULL),
        containerFormat(FORMAT_MP4),
        streamType(STREAM_TYPE_BOTH),
        maxDurationMs(120000),
        recorderInfoUpdateInterval(0),
        width(1280),
        height(720),
        fps(30),
        sample_rate(48000),
        channel_num(1),
        videoSourceType(rtc::VIDEO_SOURCE_CAMERA_PRIMARY) {}
  MediaRecorderConfiguration(const char* path, MediaRecorderContainerFormat format,
                             MediaRecorderStreamType type, int duration, int interval)
      : storagePath(path),
        containerFormat(format),
        streamType(type),
        maxDurationMs(duration),
        recorderInfoUpdateInterval(interval),
        width(1280),
        height(720),
        fps(30),
        sample_rate(48000),
        channel_num(1),
        videoSourceType(rtc::VIDEO_SOURCE_CAMERA_PRIMARY) {}
};

class IFaceInfoObserver {
 public:
  /**
   * @brief Occurs when the facial information processed by speech driven extension is received.
   *
   * @param outFaceInfo Output parameter, the JSON string of the facial information processed by the
   * voice driver plugin, including the following fields:
   * - faces: Object sequence. The collection of facial information, with each face corresponding to
   * an object.
   * - blendshapes: Object. The collection of face capture coefficients, named according to ARkit
   * standards, with each key-value pair representing a blendshape coefficient. The blendshape
   * coefficient is a floating point number with a range of [0.0, 1.0].
   * - rotation: Object sequence. The rotation of the head, which includes the following three
   * key-value pairs, with values as floating point numbers ranging from -180.0 to 180.0:
   *   - pitch: Head pitch angle. A positve value means looking down, while a negative value means
   * looking up.
   *   - yaw: Head yaw angle. A positve value means turning left, while a negative value means turning
   * right.
   *   - roll: Head roll angle. A positve value means tilting to the right, while a negative value
   * means tilting to the left.
   * - timestamp: String. The timestamp of the output result, in milliseconds.
   * Here is an example of JSON:
   * ```json
   * { "faces":[{ "blendshapes":{ "eyeBlinkLeft":0.9, "eyeLookDownLeft":0.0, "eyeLookInLeft":0.0,
   * "eyeLookOutLeft":0.0, "eyeLookUpLeft":0.0, "eyeSquintLeft":0.0, "eyeWideLeft":0.0,
   * "eyeBlinkRight":0.0, "eyeLookDownRight":0.0, "eyeLookInRight":0.0, "eyeLookOutRight":0.0,
   * "eyeLookUpRight":0.0, "eyeSquintRight":0.0, "eyeWideRight":0.0, "jawForward":0.0, "jawLeft":0.0,
   * "jawRight":0.0, "jawOpen":0.0, "mouthClose":0.0, "mouthFunnel":0.0, "mouthPucker":0.0,
   * "mouthLeft":0.0, "mouthRight":0.0, "mouthSmileLeft":0.0, "mouthSmileRight":0.0,
   * "mouthFrownLeft":0.0, "mouthFrownRight":0.0, "mouthDimpleLeft":0.0, "mouthDimpleRight":0.0,
   * "mouthStretchLeft":0.0, "mouthStretchRight":0.0, "mouthRollLower":0.0, "mouthRollUpper":0.0,
   * "mouthShrugLower":0.0, "mouthShrugUpper":0.0, "mouthPressLeft":0.0, "mouthPressRight":0.0,
   * "mouthLowerDownLeft":0.0, "mouthLowerDownRight":0.0, "mouthUpperUpLeft":0.0,
   * "mouthUpperUpRight":0.0, "browDownLeft":0.0, "browDownRight":0.0, "browInnerUp":0.0,
   * "browOuterUpLeft":0.0, "browOuterUpRight":0.0, "cheekPuff":0.0, "cheekSquintLeft":0.0,
   * "cheekSquintRight":0.0, "noseSneerLeft":0.0, "noseSneerRight":0.0, "tongueOut":0.0 },
   * "rotation":{"pitch":30.0, "yaw":25.5, "roll":-15.5},
   *             }], "timestamp":"654879876546" }
   * ```
   *
   * @return
   * - `true`: Facial information JSON parsing successful.
   * - `false`: Facial information JSON parsing failed.
   */
  virtual bool onFaceInfo(const char* outFaceInfo) = 0;

  virtual ~IFaceInfoObserver() {}
};

/**
 * @brief Information about the recording file.
 *
 * @since v3.5.2
 */
struct RecorderInfo {
  /**
   * Absolute storage path of the recording file.
   */
  const char* fileName;
  /**
   * Duration of the recording file in milliseconds.
   */
  unsigned int durationMs;
  /**
   * Size of the recording file in bytes.
   */
  unsigned int fileSize;

  RecorderInfo() : fileName(NULL), durationMs(0), fileSize(0) {}
  RecorderInfo(const char* name, unsigned int dur, unsigned int size)
      : fileName(name), durationMs(dur), fileSize(size) {}
};

class IMediaRecorderObserver {
 public:
  /**
   * @brief Callback when the recording state changes.
   *
   * @since v4.0.0
   *
   * @details
   * When the recording state of the audio and video stream changes, the SDK triggers this callback to
   * report the current recording state and the reason for the change.
   *
   * @param channelId Channel name.
   * @param uid User ID.
   * @param state Current recording state. See `RecorderState`.
   * @param reason Reason for the recording state change. See `RecorderReasonCode`.
   *
   */
  virtual void onRecorderStateChanged(const char* channelId, rtc::uid_t uid, RecorderState state,
                                      RecorderReasonCode reason) = 0;
  /**
   * @brief Callback for recording information updates.
   *
   * @since v4.0.0
   *
   * @details
   * After successfully registering this callback and starting audio and video stream recording, the
   * SDK periodically triggers this callback based on the value of `recorderInfoUpdateInterval` set in
   * `MediaRecorderConfiguration`,
   * reporting the current recording file's name, duration, and size.
   *
   * @param channelId Channel name.
   * @param uid User ID.
   * @param info Recording file information. See `RecorderInfo`.
   *
   */
  virtual void onRecorderInfoUpdated(const char* channelId, rtc::uid_t uid,
                                     const RecorderInfo& info) = 0;

  virtual ~IMediaRecorderObserver() {}
};

}  // namespace media
}  // namespace agora
