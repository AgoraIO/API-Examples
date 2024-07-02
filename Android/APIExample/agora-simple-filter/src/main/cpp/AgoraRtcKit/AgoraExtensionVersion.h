//
// Copyright (c) 2021 Agora.io. All rights reserved

// This program is confidential and proprietary to Agora.io.
// And may not be copied, reproduced, modified, disclosed to others, published
// or used, in whole or in part, without the express prior written permission
// of Agora.io.

#pragma once

namespace agora {
namespace rtc {

struct ExtensionVersion {
  // Extension Framework Version : major.minor.micro
  int major_v;
  int minor_v;
  int micro_v;

  ExtensionVersion()
    : major_v(0), minor_v(0), micro_v(0) {}
  ExtensionVersion(int majorV, int minorV = 0, int microV = 0)
    : major_v(majorV), minor_v(minorV), micro_v(microV) {}

  bool operator==(const ExtensionVersion& other) const {
    return major_v == other.major_v && minor_v == other.minor_v && micro_v == other.micro_v;
  }

  bool operator>(const ExtensionVersion& other) const {
    return major_v > other.major_v || (major_v == other.major_v && minor_v > other.minor_v)
           || (major_v == other.major_v && minor_v == other.minor_v && micro_v > other.micro_v);
  }

  bool operator<(const ExtensionVersion& other) const {
    return major_v < other.major_v || (major_v == other.major_v && minor_v < other.minor_v)
           || (major_v == other.major_v && minor_v == other.minor_v && micro_v < other.micro_v);
  }

  bool operator<=(const ExtensionVersion& other) const {
    return !operator>(other);
  }

  bool operator>=(const ExtensionVersion& other) const {
    return !operator<(other);
  }
};

#define BUMP_MAJOR_VERSION(VERSION)                                         \
  ExtensionVersion(VERSION.major_v + 1, 0, 0);                              \

#define BUMP_MINOR_VERSION(VERSION)                                         \
  ExtensionVersion(VERSION.major_v, VERSION.minor_v + 1, 0);                \

#define BUMP_MICRO_VERSION(VERSION)                                         \
  ExtensionVersion(VERSION.major_v, VERSION.minor_v, VERSION.micro_v + 1);  \

class IExtensionProvider;
class IExtensionProviderV2;
class IExtensionProviderV3;
class IAudioFilter;
class IAudioFilterV2;
class IExtensionVideoFilter;
class IScreenCaptureSource;

template <class T>
struct ExtensionInterfaceVersion;

template <>
struct ExtensionInterfaceVersion<IExtensionProvider> {
  static ExtensionVersion Version() {
    return ExtensionVersion(1, 0, 0);
  }
};

template <>
struct ExtensionInterfaceVersion<IExtensionProviderV2> {
  static ExtensionVersion Version() {
    return BUMP_MAJOR_VERSION(ExtensionInterfaceVersion<IExtensionProvider>::Version());
  }
};

template <>
struct ExtensionInterfaceVersion<IAudioFilter> {
  static ExtensionVersion Version() {
    return ExtensionVersion(1, 0, 0);
  }
};

template <>
struct ExtensionInterfaceVersion<IAudioFilterV2> {
  static ExtensionVersion Version() {
    return BUMP_MAJOR_VERSION(ExtensionInterfaceVersion<IAudioFilter>::Version());
  }
};

template <>
struct ExtensionInterfaceVersion<IExtensionVideoFilter> {
  static ExtensionVersion Version() {
    return ExtensionVersion(1, 0, 0);
  }
};

template <>
struct ExtensionInterfaceVersion<IScreenCaptureSource> {
  static ExtensionVersion Version() {
    return ExtensionVersion(1, 0, 0);
  }
};

}  // namespace rtc
}  // namespace agora
