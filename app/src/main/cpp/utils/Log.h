//
// Created by jh on 2025/3/26.
//

#pragma once

#include <android/log.h>
#include <sstream>

#ifndef LOG_TAG
#define LOG_TAG "xrgraphics"
#endif

#ifndef NDEBUG

#define LOGI(...) {                                                             \
    std::ostringstream ostr;                                                    \
    ostr<<__VA_ARGS__;                                                          \
    __android_log_write(ANDROID_LOG_INFO, LOG_TAG, ostr.str().c_str());         \
}
#define LOGD(...) {                                                             \
    std::ostringstream ostr;                                                    \
    ostr<<__VA_ARGS__;                                                          \
    __android_log_write(ANDROID_LOG_DEBUG, LOG_TAG, ostr.str().c_str());        \
}
#define LOGW(...) {                                                             \
    std::ostringstream ostr;                                                    \
    ostr<<__VA_ARGS__;                                                          \
    __android_log_write(ANDROID_LOG_WARN, LOG_TAG, ostr.str().c_str());        \
}
#define LOGE(...) {                                                             \
    std::ostringstream ostr;                                                    \
    ostr<<__VA_ARGS__;                                                          \
    __android_log_write(ANDROID_LOG_ERROR, LOG_TAG, ostr.str().c_str());        \
}

#elif

#define LOGI(...) {}
#define LOGD(...) {}
#define LOGW(...) {}
#define LOGE(...) {}

#endif