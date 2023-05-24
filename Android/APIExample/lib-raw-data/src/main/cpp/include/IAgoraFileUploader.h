
// Copyright (c) 2019 Agora.io. All rights reserved

// This program is confidential and proprietary to Agora.io.
// And may not be copied, reproduced, modified, disclosed to others, published
// or used, in whole or in part, without the express prior written permission
// of Agora.io.

#pragma once  // NOLINT(build/header_guard)

#include "AgoraRefPtr.h"

namespace agora {
namespace rtc {

struct ImagePayloadData {
  ImagePayloadData(int seq, uint8_t* pixel, int fileSize, int w, int h, int64_t ts)
      : seqid(seq),
        size(fileSize),
        width(w),
        height(h),
        timestamp(ts){
  }

  int seqid;
  int size;
  int width;
  int height;
  int64_t timestamp;
  uint8_t* buffer;
  void* privdata;
  int privsize;
};
/**
 * The IFileUploadService class, which enables upload file service.
 */
class IFileUploaderService : public RefCountInterface {
 public: 
  virtual ~IFileUploaderService() {}
  virtual int startImageUpload(const ImagePayloadData* imgData)  = 0;
  virtual int stopImageUpload() = 0;
};
}  // namespace rtc
}  // namespace agora
