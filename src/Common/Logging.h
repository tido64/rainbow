// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_LOGGING_H_
#define COMMON_LOGGING_H_

#include <cinttypes>
#include <cstdio>
#include <cstdlib>

#include "Common/Chrono.h"
#include "Platform/Macros.h"

#ifdef RAINBOW_OS_ANDROID
#   include <android/log.h>
#   define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, "Rainbow", __VA_ARGS__)
#   define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "Rainbow", __VA_ARGS__)
#   define LOGF(...) __android_log_print(ANDROID_LOG_FATAL, "Rainbow", __VA_ARGS__)
#   define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "Rainbow", __VA_ARGS__)
#   define LOGW(...) __android_log_print(ANDROID_LOG_WARN, "Rainbow", __VA_ARGS__)
#else
#   define LOG_FMT(level, fmt) "[%" PRId64 "|" level "] " fmt
#   ifdef _MSC_VER
#       define LOG_F(level, fmt, ...)                                          \
            printf(LOG_FMT(level, fmt) "\n",                                   \
                   rainbow::Chrono::system_now().count(),                      \
                   __VA_ARGS__)
#       define LOGE(fmt, ...) LOG_F("ERROR", fmt, __VA_ARGS__)
#       define LOGF(fmt, ...) LOG_F("FATAL", fmt, __VA_ARGS__)
#   else
#       define LOG_EX(...) , __VA_ARGS__
#       define LOG_F(level, fmt, ...)                                          \
            printf(LOG_FMT(level, fmt) "%c",                                   \
                   rainbow::Chrono::system_now().count()                       \
                   LOG_EX(__VA_ARGS__))
#       define LOGE(...) LOG_F("ERROR", __VA_ARGS__, '\n')
#       define LOGF(...) LOG_F("FATAL", __VA_ARGS__, '\n')
#   endif  // _MSC_VER
#   ifdef NDEBUG
#       define LOGD(...) static_cast<void>(0)
#       define LOGI(...) static_cast<void>(0)
#       define LOGW(...) static_cast<void>(0)
#   else
#       ifdef _MSC_VER
#           define LOGD(fmt, ...) LOG_F("DEBUG", fmt, __VA_ARGS__)
#           define LOGI(fmt, ...) LOG_F("INFO", fmt, __VA_ARGS__)
#           define LOGW(fmt, ...) LOG_F("WARN", fmt, __VA_ARGS__)
#       else
#           define LOGD(...) LOG_F("DEBUG", __VA_ARGS__, '\n')
#           define LOGI(...) LOG_F("INFO", __VA_ARGS__, '\n')
#           define LOGW(...) LOG_F("WARN", __VA_ARGS__, '\n')
#       endif  // _MSC_VER
#   endif  // NDEBUG
#endif  // RAINBOW_OS_ANDROID

#ifdef NDEBUG
#   define R_ABORT(...)   static_cast<void>(0)
#   define R_ASSERT(...)  static_cast<void>(0)
#else
#   ifdef _MSC_VER
#       define R_ABORT(fmt, ...) (LOGF(fmt, __VA_ARGS__), std::abort())
#   else
#       define R_ABORT(...) (LOGF(__VA_ARGS__), std::abort())
#   endif  // _MSC_VER
#   ifndef __PRETTY_FUNCTION__
#       define __PRETTY_FUNCTION__ __func__
#   endif
#   ifndef SRC_FILE
#       define SRC_FILE __FILE__
#   endif
#   define R_ASSERT(expr, reason)                                              \
        (!(expr) ? R_ABORT("%s: %s (aborted at %s:%i: %s)",                    \
                           __PRETTY_FUNCTION__,                                \
                           reason,                                             \
                           SRC_FILE,                                           \
                           __LINE__,                                           \
                           #expr)                                              \
                 : static_cast<void>(0))
#endif  // NDEBUG

#ifndef NDEBUG
#   define IF_DEBUG(expr) expr
#   define IF_DEBUG_ELSE(expr, _) expr
#else
#   define IF_DEBUG(_) static_cast<void>(0)
#   define IF_DEBUG_ELSE(_, expr) expr
#endif

#if !defined(NDEBUG) || defined(USE_HEIMDALL)
#   define RAINBOW_DEVMODE 1
#   define IF_DEVMODE(expr) expr
#else
#   define RAINBOW_DEVMODE 0
#   define IF_DEVMODE(_) static_cast<void>(0)
#endif

#endif  // COMMON_LOGGING_H_
