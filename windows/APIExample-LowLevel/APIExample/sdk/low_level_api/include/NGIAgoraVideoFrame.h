
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
 * The IVideoFrame class defines the interface to
 * send video frame data to the SDK or get video frame data from the SDK.
 */
class IVideoFrame : public RefCountInterface {
 public:
  /**
   * Supported Video Frame memory types.
   */
  OPTIONAL_ENUM_CLASS Type {
    /**
     * The frame data in CPU memory.
     */
    kRawData,
    /**
     * The frame data in GPU memory.
     */
    kTexture,
  };

  /**
   * Supported Video Frame Formats.
   */
  OPTIONAL_ENUM_CLASS Format {
    kUnknown,
    kI420,
    kI420A,
    kI422,
    kNV21,
    kNV12,
    kRGBA,
    kARGB,
    kBGRA
  };

  /**
   * Gets the Type of the frame.
   * @return
   * The type of the frame. See #Type.
   */
  virtual Type type() const = 0;

  /**
   * Gets the Format of the frame.
   * @return
   * The format of the frame. See #Format.
   */
  virtual Format format() const = 0;

  /**
   * Gets the width of the frame in pixels.
   *
   * @return
   * The width (px) of the frame.
   */
  virtual int width() const = 0;

  /**
   * Gets the height of the frame in pixels.
   *
   * @return
   * The height of the frame in pixels.
   */
  virtual int height() const = 0;

  /**
   * Gets the size of the frame in pixels
   *
   * @return
   * The size of the frame in pixels
   */
  virtual int size() const = 0;

  /**
   * Gets the rotation angle of the frame
   *
   * @return
   * Rotation angle in degrees.
   */
  virtual int rotation() const = 0;

  /**
   * Sets the rotation angle of the frame.
   *
   * @param rotation Rotation angle in degrees, such as 0, 90, 180, or 270.
   */
  virtual void setRotation(int rotation) = 0;

  /**
   * Gets the timestamp of the frame in microseconds.
   *
   * @return
   * Timestamp of the frame in microseconds.
   */
  virtual int64_t timestampUs() const = 0;

  /**
   * Sets the timestamp of the frame in microseconds.
   *
   * @param timestampUs Timestamp of the frame in microseconds.
   */
  virtual void setTimeStampUs(int64_t timestampUs) = 0;

  /**
   * Gets the constant raw data pointer to the beginning of the contigorous memory of the frame.
   *
   * @return
   * Constant pointer to the beginning of the underlying memory.
   */
  virtual const uint8_t* data() const = 0;

  /**
   * Gets the mutable raw data pointer to the beginning of the contigorous memory of the frame.
   *
   * User can modify the content of the frame's underlying memory pointed by the mutable pointer.
   * Access to memory within the scope of the frame's size is ensured to be valid.
   * @return
   * Mutable pointer to the beginning of the underlying memory.
   */
  virtual uint8_t* mutableData() = 0;

  /**
   * Resizes the buffer to new width and height.
   *
   * No reallocation happens if the new size is less than the frame's original size,
   * otherwise, a reallocation-and-copy happens.
   * @return
   * - 0: The function call succeeds.
   * - <0: The function call fails.
   */
  virtual int resize(int width, int height) = 0;

  // Reserved for future Texture data path.
  // Expect to change when hardware video frame path is established.
  struct TextureId {
    uintptr_t id_;

    TextureId() : id_(0) {}
  };

  /**
   * Gets the texture id of the underlying buffer if type id texture.
   * @return
   * - TextureId: texture id of the buffer.
   */
  virtual TextureId textureId() const = 0;

  /**
   * Fill the underlying buffer with source buffer content pointed by src and with new format, width, height & rotation.
   *
   * This function first tries to fill in-place with no copy and reallocation. When it fails, a copy or copy-plus-reallocation
   * may happen
   * @return
   * - 0: The function call succeeds.
   * - <0: The function call fails.
   */
  virtual int fill(Format format, int width, int height, int rotation, const uint8_t* src) = 0;

  /**
   * Fill the underlying buffer with new texture.
   *
   * @return
   * - 0: The function call succeeds.
   * - <0: The function call fails.
   */
  virtual int fill(Format format, int width, int height, int rotation, TextureId textureId) = 0;

 protected:
  ~IVideoFrame() {}
};

}  // namespace rtc
}  // namespace agora
