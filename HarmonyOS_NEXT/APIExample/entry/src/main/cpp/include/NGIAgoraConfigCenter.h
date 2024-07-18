//
//  Agora SDK
//
//  Copyright (c) 2023 Agora.io. All rights reserved
//

#pragma once

#include "AgoraBase.h"
#include "AgoraRefPtr.h"

namespace agora {
namespace rtc {

class IConfigCenter : public RefCountInterface {  // NOLINT
 public:
  /** Set local access point addresses in local Ap mode(which also calls local proxy).
  Use this method before join channel.
  @param config The LocalAccessPointConfiguration class, See the definition of
  LocalAccessPointConfiguration for details.
  @return
  - 0: Success
  - < 0: Failure
  */
  virtual int SetLocalAccessPoint(const LocalAccessPointConfiguration& config) = 0;

 protected:
  virtual ~IConfigCenter() {}  // NOLINT
};

}  // namespace rtc
}  // namespace agora
