// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef USE_FMOD_STUDIO

#include <cstring>

#include "Common/Debug.h"
#include "ConFuoco/AudioFile.h"

namespace ConFuoco
{
	namespace
	{
		const size_t kAudioBufferSize = 2048;
		const size_t kFallbackBufferSize = 22050;
		const int kFallbackChannels = 1;
		const int kFallbackSampleRate = kFallbackBufferSize >> 1;
	}

	size_t AudioFile::CreateBuffer(char **buffer, const unsigned int channels)
	{
		const size_t size = kAudioBufferSize * channels;
		*buffer = new char[size];
		return size;
	}

	int AudioFile::channels_impl() const
	{
		return kFallbackChannels;
	}

	int AudioFile::rate_impl() const
	{
		return kFallbackSampleRate;
	}

	size_t AudioFile::read_impl(char **dst)
	{
		R_DEBUG("[Rainbow::ConFuoco] Loading fallback\n");

		*dst = new char[kFallbackBufferSize];
		memset(*dst, 0, kFallbackBufferSize);
		return kFallbackBufferSize;
	}

	size_t AudioFile::read_impl(char *dst, const size_t size)
	{
		memset(dst, 0, size);
		return size;
	}

	void AudioFile::rewind_impl() { }
}

#endif
