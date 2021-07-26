//
// Copyright (c) 2020 Agora.io. All rights reserved

// This program is confidential and proprietary to Agora.io.
// And may not be copied, reproduced, modified, disclosed to others, published
// or used, in whole or in part, without the express prior written permission
// of Agora.io.

#pragma once  // NOLINT(build/header_guard)

#include "NGIAgoraExtensionControl.h"
AGORA_API agora::rtc::IExtensionControl* AGORA_CALL getAgoraExtensionControl();

#define DECLARE_CREATE_AND_REGISTER_PROVIDER(PROVIDER_NAME, PROVIDER_CLASS, ...)          \
static void register_##PROVIDER_NAME##_to_agora() {                                       \
  auto control = getAgoraExtensionControl();                                              \
  if (#PROVIDER_NAME && control) {                                                        \
    control->registerProvider(#PROVIDER_NAME,                                             \
                              new agora::RefCountedObject<PROVIDER_CLASS>(__VA_ARGS__));  \
  }                                                                                       \
}                                                                                         \

#if defined (__GNUC__) && defined (TARGET_OS_IPHONE)
#define REGISTER_AGORA_EXTENSION_PROVIDER(PROVIDER_NAME, PROVIDER_CLASS, ...)             \
DECLARE_CREATE_AND_REGISTER_PROVIDER(PROVIDER_NAME, PROVIDER_CLASS, __VA_ARGS__);         \
typedef void(*agora_ext_entry_func_t)(void);                                              \
AGORA_API void AGORA_CALL registerProvierEntry(const char*, agora_ext_entry_func_t);      \
__attribute__((constructor, used))                                                        \
static void _##PROVIDER_NAME##_provider_entry() {                                         \
  registerProvierEntry(#PROVIDER_NAME, register_##PROVIDER_NAME##_to_agora);              \
}                                                                                         \

#elif defined (__GNUC__) && (defined (__ANDROID__) || (__linux__))
#define REGISTER_AGORA_EXTENSION_PROVIDER(PROVIDER_NAME, PROVIDER_CLASS, ...)             \
DECLARE_CREATE_AND_REGISTER_PROVIDER(PROVIDER_NAME, PROVIDER_CLASS, __VA_ARGS__);         \
__attribute__((constructor, used))                                                        \
static void _##PROVIDER_NAME##_provider_entry() {                                         \
  register_##PROVIDER_NAME##_to_agora();                                                  \
}                                                                                         \

#elif defined (_MSC_VER)
#define REGISTER_AGORA_EXTENSION_PROVIDER(PROVIDER_NAME, PROVIDER_CLASS, ...)             \
DECLARE_CREATE_AND_REGISTER_PROVIDER(PROVIDER_NAME, PROVIDER_CLASS, __VA_ARGS__);         \
BOOL APIENTRY DllMain( HMODULE hModule,                                                   \
                       DWORD  ul_reason_for_call,                                         \
                       LPVOID lpReserved) {                                               \
  switch (ul_reason_for_call) {                                                           \
    case DLL_PROCESS_ATTACH:                                                              \
      register_##PROVIDER_NAME##_to_agora();                                              \
      break;                                                                              \
    default:                                                                              \
      break;                                                                              \
  }                                                                                       \
  return TRUE;                                                                            \
}                                                                                         \

#define REGISTER_AGORA_EXTENSION_PROVIDER_WITH_CUSTOM_DLLMAIN(PROVIDER_NAME, PROVIDER_CLASS, DLLMAINFUNC, ...)  \
DECLARE_CREATE_AND_REGISTER_PROVIDER(PROVIDER_NAME, PROVIDER_CLASS, __VA_ARGS__);                               \
BOOL APIENTRY DllMain( HMODULE hModule,                                                                         \
                       DWORD  ul_reason_for_call,                                                               \
                       LPVOID lpReserved) {                                                                     \
  if (!DLLMAINFUNC(hModule, ul_reason_for_call, lpReserved)) {                                                  \
    return FALSE;                                                                                               \
  }                                                                                                             \
  switch (ul_reason_for_call) {                                                                                 \
    case DLL_PROCESS_ATTACH:                                                                                    \
      register_##PROVIDER_NAME##_to_agora();                                                                    \
      break;                                                                                                    \
    default:                                                                                                    \
      break;                                                                                                    \
  }                                                                                                             \
  return TRUE;                                                                                                  \
}                                                                                                               \


#else
#error Unsupported Compilation Toolchain!
#endif
