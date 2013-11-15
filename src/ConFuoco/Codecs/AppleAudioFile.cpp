#include "ConFuoco/Codecs/AppleAudioFile.h"
#if defined(RAINBOW_OS_IOS) || defined(RAINBOW_OS_MACOS)

#include "Common/Debug.h"
#include "ConFuoco/Codecs/OggVorbisAudioFile.h"
#include "FileSystem/File.h"
#include "FileSystem/Path.h"

#define strllen(literal) sizeof(literal) / sizeof(char) - sizeof(char)

namespace ConFuoco
{
	namespace
	{
		const size_t kAudioBufferSize = 8192;
		const unsigned int kAudioChannelsMono = 1;
		const unsigned int kAudioChannelsStereo = 2;
		const unsigned int kBitsPerChannel = 16;

		template<class C, class T>
		C bridge_cast(T &var)
		{
		#ifdef RAINBOW_OS_IOS
			return (__bridge C)var;
		#else
			return (C)var;
		#endif
		}
	}

	AudioFile* AudioFile::Open(const char *const file, const Mode mode)
	{
	#ifndef RAINBOW_OS_IOS
		char id[8] = { 0 };
		File f = File::open_asset(file);
		if (f)
		{
			f.read(id, sizeof(id));
			f.seek(0, SEEK_SET);
		}
		if (strncmp(Codecs::kCodecOggVorbis, id, strllen(Codecs::kCodecOggVorbis)) == 0)
			return new OggVorbisAudioFile(std::move(f));
		else
	#endif
			return new AppleAudioFile(file, mode);
	}

	AppleAudioFile::AppleAudioFile(const char *const file, const int mode) : ref(nullptr)
	{
		const Path path(file);
	#ifdef RAINBOW_OS_MACOS
		CFURLRef url = path.CreateCFURL();
	#else
		NSURL *url = path;
	#endif
		if (ExtAudioFileOpenURL(bridge_cast<CFURLRef>(url), &this->ref) != noErr)
			R_ERROR("[Rainbow::ConFuoco/AudioToolbox] Failed to open '%s'\n", file);
	#ifdef RAINBOW_OS_MACOS
		CFRelease(url);
	#endif
		if (!this->ref)
			return;

		UInt32 size = sizeof(this->format);
		memset(&this->format, 0, size);
		if (ExtAudioFileGetProperty(
				this->ref, kExtAudioFileProperty_FileDataFormat, &size,
				&this->format) != noErr)
			R_ERROR("[Rainbow::ConFuoco/AudioToolbox] Failed to retrieve audio format\n");

		FillOutASBDForLPCM(
				this->format,
				this->format.mSampleRate,
				(mode == kAudioFileStream) ? this->format.mChannelsPerFrame : kAudioChannelsMono,
				kBitsPerChannel,
				kBitsPerChannel,
				false,
				false);
		if (ExtAudioFileSetProperty(
				this->ref, kExtAudioFileProperty_ClientDataFormat,
				sizeof(this->format), &this->format) != noErr)
			R_ERROR("[Rainbow::ConFuoco/AudioToolbox] Failed to set client data format\n");
	}

	AppleAudioFile::~AppleAudioFile()
	{
		if (this->ref)
			ExtAudioFileDispose(this->ref);
	}

	int AppleAudioFile::channels_impl() const
	{
		if (!this->ref)
			return AudioFile::channels_impl();

		return this->format.mChannelsPerFrame;
	}

	int AppleAudioFile::rate_impl() const
	{
		if (!this->ref)
			return AudioFile::rate_impl();

		return this->format.mSampleRate;
	}

	size_t AppleAudioFile::read_impl(char **dst)
	{
		if (!this->ref)
			return AudioFile::read_impl(dst);

		SInt64 frames = 0;
		UInt32 size = sizeof(frames);
		if (ExtAudioFileGetProperty(
				this->ref, kExtAudioFileProperty_FileLengthFrames, &size,
				&frames) != noErr)
		{
			R_ERROR("[Rainbow::ConFuoco/AudioToolbox] Failed to retrieve audio length\n");
			return 0;
		}
		size = frames * this->format.mBytesPerFrame;
		*dst = new char[size];
		return this->read(*dst, size);
	}

	size_t AppleAudioFile::read_impl(char *dst, const size_t size)
	{
		if (!this->ref)
			return AudioFile::read_impl(dst, size);

		UInt32 frames = size / this->format.mBytesPerFrame;
		AudioBufferList buffer;
		buffer.mNumberBuffers = 1;
		buffer.mBuffers[0].mNumberChannels = this->format.mChannelsPerFrame;
		buffer.mBuffers[0].mDataByteSize = size;
		buffer.mBuffers[0].mData = dst;
		if (ExtAudioFileRead(this->ref, &frames, &buffer) != noErr)
			R_ERROR("[Rainbow::ConFuoco/AudioToolbox] Failed to read <%p>\n",this->ref);
		return frames * this->format.mBytesPerFrame;
	}

	void AppleAudioFile::rewind_impl()
	{
		if (!this->ref)
			return AudioFile::rewind_impl();

		ExtAudioFileSeek(this->ref, 0);
	}
}

#undef strllen
#endif
