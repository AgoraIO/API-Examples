/**
 *
 * Agora Real Time Engagement
 * Copyright (c) 2024 Agora IO. All rights reserved.
 *
 */
#pragma once

#include <stdbool.h>

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus

typedef struct RteString RteString;

/**
 * @brief RTE-related status and error codes.
 */
typedef enum RteErrorCode {
  /**
   * 0: Operation succeeded.
   */
  kRteOk,
  /**
   * 1: General error (no specific cause).
   */
  kRteErrorDefault,
  /**
   * 2: Invalid parameter in the method. For example, the parameter contains illegal characters or the
   * passed object is null. Please reset the parameter.
   */
  kRteErrorInvalidArgument,
  /**
   * 3: The current state does not support the operation, such as the object being destroyed or
   * invalid.
   */
  kRteErrorInvalidOperation,
  /**
   * 4: Network error. Please check your network.
   */
  kRteErrorNetworkError,
  /**
   * 5: Token authentication failed. Possible reasons include an invalid or expired token. Please
   * check your token.
   */
  kRteErrorAuthenticationFailed,
  /**
   * 6: Media stream not found. For example, no media stream is received from the host within 10
   * seconds after joining the channel.
   */
  kRteErrorStreamNotFound,
} RteErrorCode;

typedef struct RteError {
  RteErrorCode code;
  RteString *message;
} RteError;

AGORA_RTE_API_C RteError *RteErrorCreate();
AGORA_RTE_API_C bool RteErrorDestroy(RteError *err);

AGORA_RTE_API_C bool RteErrorInit(RteError *err);
AGORA_RTE_API_C bool RteErrorDeinit(RteError *err);

AGORA_RTE_API_C bool RteErrorCopy(RteError *dest, RteError *src);

AGORA_RTE_API_C bool RteErrorSet(RteError *err, RteErrorCode code,
                                const char *fmt, ...);
AGORA_RTE_API_C bool RteErrorOccurred(RteError *err);

#ifdef __cplusplus
}
#endif  // __cplusplus
