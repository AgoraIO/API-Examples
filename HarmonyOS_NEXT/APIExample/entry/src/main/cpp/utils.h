//
// Created on 2024/6/16.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#pragma once

#include <hilog/log.h>
#define NATIVE_TAG "RawDataProcessing"

#define AG_DEBUG(...) OH_LOG_Print(LogType::LOG_APP, LogLevel::LOG_DEBUG, 0x1, NATIVE_TAG, __VA_ARGS__)
#define AG_INFO(...) OH_LOG_Print(LogType::LOG_APP, LogLevel::LOG_INFO, 0x1, NATIVE_TAG, __VA_ARGS__)
#define AG_ERROR(...) OH_LOG_Print(LogType::LOG_APP, LogLevel::LOG_ERROR, 0x1, NATIVE_TAG, __VA_ARGS__)
