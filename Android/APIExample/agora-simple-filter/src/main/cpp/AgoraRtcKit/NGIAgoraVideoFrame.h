
// Copyright (c) 2020 Agora.io. All rights reserved

// This program is confidential and proprietary to Agora.io.
// And may not be copied, reproduced, modified, disclosed to others, published
// or used, in whole or in part, without the express prior written permission
// of Agora.io.

#pragma once  // NOLINT(build/header_guard)

#include "AgoraBase.h"

#ifndef OPTIONAL_ENUM_CLASS
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1800)
#define OPTIONAL_ENUM_CLASS enum class
#else
#define OPTIONAL_ENUM_CLASS enum
#endif
#endif

namespace agora {
namespace rtc {

/**
 * This structure defines the video frame of texture type on Android
 * @note For technical preview, not supported for the moment. Use RawPixelBuffer instead.
 * 
 */
struct TextureInfo {
  OPTIONAL_ENUM_CLASS TextureType {
    kGlTextureOes,
    kGlTexture2D,
  };
  OPTIONAL_ENUM_CLASS EglContextType {
    kEglContext10,
    kEglContext14,
  };

  TextureType texture_type;
  EglContextType context_type;
  void* shared_context;
  int texture_id;
  float transform_matrix[16];
};

/**
 * This structure defines the raw video frame data in memory
 * 
 */
struct RawPixelBuffer {
  OPTIONAL_ENUM_CLASS Format {
    kUnknown,
    kI420,
    kI422,
    kNV21,
    kNV12,
    kI010,
    kRGBA,
    kARGB,
    kBGRA
  };
  Format format;
  uint8_t* data;
  int size;
};

struct PaddedRawPixelBuffer {
  RawPixelBuffer::Format format;
  uint8_t* data;
  int size;
  int stride;
  PaddedRawPixelBuffer()
    : data(NULL), size(0), stride(0) {}
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
};

/**
 * This structure defines underlying detailed video frame data of @ref agora::rtc::IVideoFrame
 * 
 */
struct VideoFrameData {
  OPTIONAL_ENUM_CLASS Type {
    kRawPixels, // Raw pixels in memory
    kTexture, // Deprecated Android: GL_TEXTURE_2D/GL_TEXTURE_OES
    kCVPixelBuffer, // iOS: CVPixelBufferRef
    kPaddedRawPixels, // Raw pixels with paddings
    kTextureOES,// Android: GL_TEXTURE_OES
    kTexture2D, // Android: GL_TEXTURE_2D
  };
  Type type;
  union {
    TextureInfo texture; // Android (To be supported)
    RawPixelBuffer pixels; // All platform
    void* cvpixelbuffer; // iOS (To be supported)
  };
  int width;
  int height;
  int rotation;
  ColorSpace color_space;
  int64_t timestamp_ms; // Capture time in milli-seconds
};

struct VideoFrameDataV2 : public VideoFrameData {
  PaddedRawPixelBuffer padded_pixels; // All platform
};

OPTIONAL_ENUM_CLASS VideoFrameMetaDataType {
  kAlphaChannel,
  kScreenMetaInfo,
  kVideoSourceType,
  kFaceInfo,
  kFaceCaptureInfo,
  // Add other types afterwards
};

struct AlphaChannel {
  uint8_t* data;
  int size;
};

typedef int32_t VideoSourceType;

/**
 * The IVideoFrame class defines the interface to
 * send video frame data to the SDK or get video frame data from the SDK.
 */
class IVideoFrame : public RefCountInterface {
 public:
  /**
   * Get the concrete video frame data of the underlying buffer.
   * @param data [out] The video frame data.
   * @return
   * - int: error code. 0 for success.
   */
  virtual int getVideoFrameData(VideoFrameData& data) const = 0;

  /**
   * Fill the underlying buffer with source buffer info contained in VideoFrameData
   * For frames of type "Type::kMemPixels", This function first tries to fill in-place with no copy and reallocation.
   * When it fails, a copy or copy-plus-reallocation may happen
   * @param data [in] Data to be filled in.
   * @return 
   * - 0: if succeeds
   * - <0: failure
   */
  virtual int fillVideoFrameData(const VideoFrameData& data) = 0;

  /**
   * Get the underlying meta data of the given type.
   * e.g. If type is VideoFrameMetaDataType::kAlphaChannel, then data value can be casted to AlphaChannel;
   * @param type @ref VideoFrameMetaDataType type of the meta data wanted
   * @param data [out] pointer to the the meta data object of the given type
   * @return 
   * - 0: if succeeds
   * - <0: failure
   */
  virtual int getVideoFrameMetaData(VideoFrameMetaDataType type, void* data) = 0;

  /**
   * Fill the underlying meta data of the given type.
   * e.g. If type is VideoFrameMetaDataType::kAlphaChannel, then data should point to AlphaChannel object;
   * @param type @ref VideoFrameMetaDataType type of the meta data to fill
   * @param data [in] pointer to the the meta data object of the given type
   * @return 
   * - 0: if succeeds
   * - <0: failure
   */
  virtual int fillVideoFrameMetaData(VideoFrameMetaDataType type, const void* data) = 0;

 protected:
  ~IVideoFrame() {}
};

class IVideoFrameMemoryPool : public RefCountInterface {
 public:
  /**
   * Create IVideoFrame
   * @param data video frame data of the new video frame
   * @param metatypes pointer to the array of meta types
   * @param count number of the meta types in the array
   */
  virtual agora::agora_refptr<IVideoFrame> createVideoFrame(
      const VideoFrameData& data, const VideoFrameMetaDataType* metatypes = NULL, int count = 0) = 0;

 protected:
  virtual ~IVideoFrameMemoryPool() {}
};

}  // namespace rtc
}  // namespace agora
