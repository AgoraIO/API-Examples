//
// Copyright (c) 2020 Agora.io. All rights reserved

// This program is confidential and proprietary to Agora.io.
// And may not be copied, reproduced, modified, disclosed to others, published
// or used, in whole or in part, without the express prior written permission
// of Agora.io.

#pragma once  // NOLINT(build/header_guard)

#include "NGIAgoraExtensionControl.h"
AGORA_API agora::rtc::IExtensionControl* AGORA_CALL getAgoraExtensionControl();
AGORA_API void AGORA_CALL declareProviderVersion(
        const char*, const agora::rtc::ExtensionVersion&);
typedef void(*agora_ext_entry_func_t)(void);
AGORA_API void AGORA_CALL registerProviderEntry(const char*, agora_ext_entry_func_t);

#define DECLARE_CREATE_AND_REGISTER_PROVIDER(PROVIDER_NAME, PROVIDER_CLASS, PROVIDER_INTERFACE_USED, ...)  \
static void register_##PROVIDER_NAME##_to_agora() {                                                        \
  auto control = getAgoraExtensionControl();                                                               \
   agora::rtc::ExtensionVersion version =                                                                  \
      agora::rtc::ExtensionInterfaceVersion<PROVIDER_INTERFACE_USED>::Version();                           \
  declareProviderVersion(#PROVIDER_NAME, version);                                                         \
  if (#PROVIDER_NAME && control) {                                                                         \
    control->registerProvider(#PROVIDER_NAME,                                                              \
                              new agora::RefCountedObject<PROVIDER_CLASS>(__VA_ARGS__));                   \
  }                                                                                                        \
}                                                                                                          \

#define DECLARE_CREATE_AND_REGISTER_PROVIDER_PTR(PROVIDER_NAME, PROVIDER_INTERFACE_USED, PROVIDER_REF_PTR) \
static void register_##PROVIDER_NAME##_to_agora() {                                                        \
  auto control = getAgoraExtensionControl();                                                               \
   agora::rtc::ExtensionVersion version =                                                                  \
      agora::rtc::ExtensionInterfaceVersion<PROVIDER_INTERFACE_USED>::Version();                           \
  declareProviderVersion(#PROVIDER_NAME, version);                                                         \
  if (#PROVIDER_NAME && control) {                                                                         \
    control->registerProvider(#PROVIDER_NAME, PROVIDER_REF_PTR);                                           \
  }                                                                                                        \
}                                                                                                          \


#if defined (__GNUC__)
#define REGISTER_AGORA_EXTENSION_PROVIDER(PROVIDER_NAME, PROVIDER_CLASS, PROVIDER_INTERFACE_USED, ...)     \
DECLARE_CREATE_AND_REGISTER_PROVIDER(PROVIDER_NAME, PROVIDER_CLASS, PROVIDER_INTERFACE_USED, __VA_ARGS__); \
__attribute__((constructor, used))                                                                         \
static void _##PROVIDER_NAME##_provider_entry() {                                                          \
  registerProviderEntry(#PROVIDER_NAME, register_##PROVIDER_NAME##_to_agora);                              \
}                                                                                                          \

#define REGISTER_AGORA_EXTENSION_PROVIDER_PTR(PROVIDER_NAME, PROVIDER_INTERFACE_USED, PROVIDER_REF_PTR)    \
DECLARE_CREATE_AND_REGISTER_PROVIDER_PTR(PROVIDER_NAME, PROVIDER_INTERFACE_USED, PROVIDER_REF_PTR);        \
__attribute__((constructor, used))                                                                         \
static void _##PROVIDER_NAME##_provider_entry() {                                                          \
  registerProviderEntry(#PROVIDER_NAME, register_##PROVIDER_NAME##_to_agora);                              \
}                                                                                                          \


#elif defined (_MSC_VER)
#define REGISTER_AGORA_EXTENSION_PROVIDER(PROVIDER_NAME, PROVIDER_CLASS, PROVIDER_INTERFACE_USED, ...)     \
DECLARE_CREATE_AND_REGISTER_PROVIDER(PROVIDER_NAME, PROVIDER_CLASS, PROVIDER_INTERFACE_USED, __VA_ARGS__); \
static int _##PROVIDER_NAME##_provider_entry() {                                                           \
  registerProviderEntry(#PROVIDER_NAME, register_##PROVIDER_NAME##_to_agora);                              \
  return 0;                                                                                                \
}                                                                                                          \
const int DUMMY_AGORA_REGEXT_##PROVIDE_NAME##_VAR = _##PROVIDER_NAME##_provider_entry();                   \

#define REGISTER_AGORA_EXTENSION_PROVIDER_PTR(PROVIDER_NAME, PROVIDER_INTERFACE_USED, PROVIDER_REF_PTR)    \
DECLARE_CREATE_AND_REGISTER_PROVIDER_PTR(PROVIDER_NAME, PROVIDER_INTERFACE_USED, PROVIDER_REF_PTR);        \
static int _##PROVIDER_NAME##_provider_entry() {                                                           \
  registerProviderEntry(#PROVIDER_NAME, register_##PROVIDER_NAME##_to_agora);                              \
  return 0;                                                                                                \
}                                                                                                          \
const int DUMMY_AGORA_REGEXT_##PROVIDE_NAME##_VAR = _##PROVIDER_NAME##_provider_entry();                   \

#else
#error Unsupported Compilation Toolchain!
#endif
