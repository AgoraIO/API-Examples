//
// Created by 张涛 on 2020/4/26.
//

#ifndef AGORAWITHBYTEDANCE_LOGUTILS_H
#define AGORAWITHBYTEDANCE_LOGUTILS_H
#include <android/log.h>
#define LOG_TAG "Agora_zt C++"
#define PRINTF_INFO(...) __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG, __VA_ARGS__)
#define PRINTF_ERROR(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG, __VA_ARGS__)
#define PRINT_API_CALL(...) PRINTF_INFO("[api] %s , %s", __FUNCTION__, __VA_ARGS__)
#endif //AGORAWITHBYTEDANCE_LOGUTILS_H
