//
//  Agora Media SDK
//
//  Copyright (c) 2015 Agora IO. All rights reserved.
//
#pragma once

#include <cstdlib>
#include <stdint.h>

#ifndef OPTIONAL_ENUM_CLASS
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1800)
#define OPTIONAL_ENUM_CLASS enum class
#else
#define OPTIONAL_ENUM_CLASS enum
#endif
#endif

#ifndef OPTIONAL_LOG_LEVEL_SPECIFIER
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1800)
#define OPTIONAL_LOG_LEVEL_SPECIFIER LOG_LEVEL::
#else
#define OPTIONAL_LOG_LEVEL_SPECIFIER
#endif
#endif

namespace agora {
namespace commons {

/**
 * @brief The output log level of the SDK.
 */
OPTIONAL_ENUM_CLASS LOG_LEVEL {
  /**
   * 0: Do not output any log information.
   */
  LOG_LEVEL_NONE = 0x0000,
  /**
   * 0x0001: (Default) Output `FATAL`, `ERROR`, `WARN`, and `INFO` level log information. We recommend
   * setting your log filter to this level.
   */
  LOG_LEVEL_INFO = 0x0001,
  /**
   * 0x0002: Output `FATAL`, `ERROR`, and `WARN` level log information.
   */
  LOG_LEVEL_WARN = 0x0002,
  /**
   * 0x0004: Output `FATAL` and `ERROR` level log information.
   */
  LOG_LEVEL_ERROR = 0x0004,
  /**
   * 0x0008: Output `FATAL` level log information.
   */
  LOG_LEVEL_FATAL = 0x0008,
  LOG_LEVEL_API_CALL = 0x0010,
  LOG_LEVEL_DEBUG = 0x0020,
};

/*
The SDK uses ILogWriter class Write interface to write logs as application
The application inherits the methods Write() to implentation their own  log writ

Write has default implementation, it writes logs to files.
Application can use setLogFile() to change file location, see description of set
*/
class ILogWriter {
 public:
  /** user defined log Write function
  @param level log level
  @param message log message content
  @param length log message length
  @return
   - 0: success
   - <0: failure
  */
  virtual int32_t writeLog(LOG_LEVEL level, const char* message, uint16_t length) = 0;

  virtual ~ILogWriter() {}
};

/**
 * @brief The output log level of the SDK.
 */
enum LOG_FILTER_TYPE {
  /**
   * 0: Do not output any log information.
   */
  LOG_FILTER_OFF = 0,
  /**
   * 0x080f: Output all log information. Set your log filter to this level if you want to get the most
   * complete log file.
   */
  LOG_FILTER_DEBUG = 0x080f,
  /**
   * 0x000f: Output `LOG_FILTER_CRITICAL`, `LOG_FILTER_ERROR`, `LOG_FILTER_WARN`, and
   * `LOG_FILTER_INFO` level log information. We recommend setting your log filter to this level.
   */
  LOG_FILTER_INFO = 0x000f,
  /**
   * 0x000e: Output `LOG_FILTER_CRITICAL`, `LOG_FILTER_ERROR`, and `LOG_FILTER_WARN` level log
   * information.
   */
  LOG_FILTER_WARN = 0x000e,
  /**
   * 0x000c: Output `LOG_FILTER_CRITICAL` and `LOG_FILTER_ERROR` level log information.
   */
  LOG_FILTER_ERROR = 0x000c,
  /**
   * 0x0008: Output `LOG_FILTER_CRITICAL` level log information.
   */
  LOG_FILTER_CRITICAL = 0x0008,
  LOG_FILTER_MASK = 0x80f,
};

const uint32_t MAX_LOG_SIZE = 20 * 1024 * 1024;  // 20MB
const uint32_t MIN_LOG_SIZE = 128 * 1024;        // 128KB
/** The default log size in kb
 */
const uint32_t DEFAULT_LOG_SIZE_IN_KB = 2048;

/**
 * @brief Configuration of Agora SDK log files.
 */
struct LogConfig {
  /**
   * The complete path of the log files. Agora recommends using the default log directory. If you need
   * to modify the default directory, ensure that the directory you specify exists and is writable.
   * The default log directory is:
   * - Android: /storage/emulated/0/Android/data/<packagename>/files/agorasdk.log.
   * - iOS: App Sandbox/Library/caches/agorasdk.log.
   * - macOS:
   *   - If Sandbox is enabled: App Sandbox/Library/Logs/agorasdk.log. For example,
   * /Users/<username>/Library/Containers/<AppBundleIdentifier>/Data/Library/Logs/agorasdk.log.
   *   - If Sandbox is disabled: ~/Library/Logs/agorasdk.log
   * - Windows: C:\Users\<user_name>\AppData\Local\Agora\<process_name>\agorasdk.log.
   */
  const char* filePath;
  /**
   * The size (KB) of an `agorasdk.log` file. The value range is [128,20480]. The default value is
   * 2,048 KB. If you set `fileSizeInKByte` smaller than 128 KB, the SDK automatically adjusts it to
   * 128 KB; if you set `fileSizeInKByte` greater than 20,480 KB, the SDK automatically adjusts it to
   * 20,480 KB.
   */
  uint32_t fileSizeInKB;
  /**
   * The output level of the SDK log file. See `LOG_LEVEL`.
   * For example, if you set the log level to WARN, the SDK outputs the logs within levels FATAL,
   * ERROR, and WARN.
   */
  LOG_LEVEL level;

  LogConfig() : filePath(NULL), fileSizeInKB(DEFAULT_LOG_SIZE_IN_KB), level(OPTIONAL_LOG_LEVEL_SPECIFIER LOG_LEVEL_INFO) {}
};
}  // namespace commons
}  // namespace agora

#undef OPTIONAL_LOG_LEVEL_SPECIFIER
