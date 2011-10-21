/// Copyright 2011 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#include "Common/Chrono.h"

#if defined(RAINBOW_WIN) && !defined(RAINBOW_SDL)

namespace Rainbow
{
	static struct chrono_info
	{
		bool hiperf_clock = false;
		unsigned int freq_to_ms = 10000;
		LARGE_INTEGER offset;
	} ChronoInfo;
}

Chrono::Chrono() : tm_current(0), tm_dt(0), tm_previous(0)
{
	LARGE_INTEGER perf_freq;
	Rainbow::ChronoInfo.hiperf_clock = QueryPerformanceFrequency(perf_freq);
	if (!Rainbow::ChronoInfo.hiperf_clock)
		Rainbow::ChronoInfo.offset = getFILETIMEoffset();
	else
	{
		QueryPerformanceCounter(&Rainbow::ChronoInfo.offset);
		Rainbow::ChronoInfo.freq_to_ms = perf_freq.QuadPart / 1e9;
	}
}

unsigned long Chrono::get_time()
{
	LARGE_INTEGER t;

	if (!Rainbow::ChronoInfo.hiperf_clock)
	{
		FILETIME ft;
		GetSystemTimeAsFileTime(&ft);
		t.QuadPart = ft.dwHighDateTime << 32;
		t.QuadPart |= ft.dwLowDateTime;
	}
	else
		QueryPerformanceCounter(&t);

	t.QuadPart -= Rainbow::ChronoInfo.offset.QuadPart;
	return t.QuadPart / freq_to_ms;
}

#endif
