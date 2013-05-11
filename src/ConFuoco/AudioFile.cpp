#include <cstddef>
#include <cstring>

#include "Common/Debug.h"
#include "Common/IO.h"
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

	AudioFile::~AudioFile() { }

	int AudioFile::get_channels_impl() const
	{
		return kFallbackChannels;
	}

	int AudioFile::get_rate_impl() const
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

	size_t AudioFile::read_impl(char *, const size_t)
	{
		return 0;
	}

	void AudioFile::rewind_impl() { }
}
