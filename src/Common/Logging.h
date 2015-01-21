// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_LOGGING_H_
#define COMMON_LOGGING_H_

#include <cstdio>
#include <cstdlib>
#include <utility>

#include "Common/Chrono.h"
#include "Platform/Macros.h"

#ifdef RAINBOW_OS_ANDROID
#	include <android/log.h>
#	define RAINBOW_LOG_DEBUG  ANDROID_LOG_DEBUG
#	define RAINBOW_LOG_ERROR  ANDROID_LOG_ERROR
#	define RAINBOW_LOG_FATAL  ANDROID_LOG_FATAL
#	define RAINBOW_LOG_INFO   ANDROID_LOG_INFO
#	define RAINBOW_LOG_WARN   ANDROID_LOG_WARN
#else
#	define RAINBOW_LOG_DEBUG  "DEBUG"
#	define RAINBOW_LOG_ERROR  "ERROR"
#	define RAINBOW_LOG_FATAL  "FATAL"
#	define RAINBOW_LOG_INFO   "INFO"
#	define RAINBOW_LOG_WARN   "WARN"
#	if defined(_MSC_VER) && _MSC_VER <= 1800
#		define snprintf(...) sprintf_s(__VA_ARGS__)
#	endif
#endif  // RAINBOW_OS_ANDROID

#define LOGE(...) rainbow::error(__VA_ARGS__)
#ifdef NDEBUG
#	define LOGD(...)      static_cast<void>(0)
#	define LOGF(...)      static_cast<void>(0)
#	define LOGI(...)      static_cast<void>(0)
#	define LOGW(...)      static_cast<void>(0)
#	define R_ABORT(...)   static_cast<void>(0)
#	define R_ASSERT(...)  static_cast<void>(0)
#else
#	define LOGD(...) rainbow::debug(__VA_ARGS__)
#	define LOGF(...) rainbow::fatal(__VA_ARGS__)
#	define LOGI(...) rainbow::info(__VA_ARGS__)
#	define LOGW(...) rainbow::warn(__VA_ARGS__)
#	ifndef __PRETTY_FUNCTION__
#		define __PRETTY_FUNCTION__ __FUNCTION__
#	endif
#	ifndef SRC_FILE
#		define SRC_FILE __FILE__
#	endif
#	define R_ABORT(...) rainbow::abort(__VA_ARGS__)
#	define R_ASSERT(expr, reason) \
		if (!(expr)) \
			rainbow::abort("%s: %s (aborted at %s:%i: %s)", \
			               __PRETTY_FUNCTION__, \
			               reason, \
			               SRC_FILE, \
			               __LINE__, \
			               #expr)
#endif  // NDEBUG

namespace rainbow
{
	const size_t kLogLineLength = 1024;

	template<typename T, size_t N, typename... Args>
	void print(FILE *stream, T level, const char (&format)[N], Args&&... args)
	{
#ifdef __GNUC__
#	pragma GCC diagnostic push
#	pragma GCC diagnostic ignored "-Wformat-security"
#endif
#ifdef RAINBOW_OS_ANDROID
		static_cast<void>(stream);
		__android_log_print(
		    level, "Rainbow", format, std::forward<Args>(args)...);
#else
		const long long int timestamp = Chrono::system_now().count();
		char buf[kLogLineLength];
		snprintf(buf, kLogLineLength, format, std::forward<Args>(args)...);
		fprintf(stream, "[%lli|%s] %s\n", timestamp, level, buf);
#endif  // RAINBOW_OS_ANDROID
#ifdef __GNUC__
#	pragma GCC diagnostic pop
#endif
	}

	template<size_t N, typename... Args>
	void error(const char (&format)[N], Args&&... args)
	{
		print(stderr, RAINBOW_LOG_ERROR, format, std::forward<Args>(args)...);
	}

#ifndef NDEBUG
	template<size_t N, typename... Args>
	void debug(const char (&format)[N], Args&&... args)
	{
		print(stdout, RAINBOW_LOG_DEBUG, format, std::forward<Args>(args)...);
	}

	template<size_t N, typename... Args>
	void fatal(const char (&format)[N], Args&&... args)
	{
		print(stderr, RAINBOW_LOG_FATAL, format, std::forward<Args>(args)...);
	}

	template<size_t N, typename... Args>
	void info(const char (&format)[N], Args&&... args)
	{
		print(stdout, RAINBOW_LOG_INFO, format, std::forward<Args>(args)...);
	}

	template<size_t N, typename... Args>
	void warn(const char (&format)[N], Args&&... args)
	{
		print(stderr, RAINBOW_LOG_WARN, format, std::forward<Args>(args)...);
	}

	template<size_t N, typename... Args>
	void abort(const char (&format)[N], Args&&... args)
	{
		fatal(format, std::forward<Args>(args)...);
		std::abort();
	}
#endif  // !NDEBUG
}

#undef RAINBOW_LOG_DEBUG
#undef RAINBOW_LOG_ERROR
#undef RAINBOW_LOG_FATAL
#undef RAINBOW_LOG_INFO
#undef RAINBOW_LOG_WARN

#endif  // RAINBOW_LOGGING_H_
