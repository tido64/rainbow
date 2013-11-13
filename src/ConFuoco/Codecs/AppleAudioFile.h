#ifndef CONFUOCO_CODECS_APPLEAUDIOFILE_H_
#define CONFUOCO_CODECS_APPLEAUDIOFILE_H_

#include "Platform/Macros.h"
#if defined(RAINBOW_OS_IOS) || defined(RAINBOW_OS_MACOS)

#include <AudioToolbox/ExtendedAudioFile.h>

#include "ConFuoco/AudioFile.h"

namespace ConFuoco
{
	/// Extended Audio File Services implementation of AudioFile.
	///
	/// Copyright 2013 Bifrost Entertainment. All rights reserved.
	class AppleAudioFile : public AudioFile
	{
	public:
		AppleAudioFile(const char *const file, const int mode);
		virtual ~AppleAudioFile();

	private:
		AudioStreamBasicDescription format;
		ExtAudioFileRef ref;

		/* Implement AudioFile */

		virtual int channels_impl() const override;
		virtual int rate_impl() const override;
		virtual size_t read_impl(char **dst) override;
		virtual size_t read_impl(char *dst, const size_t size) override;
		virtual void rewind_impl() override;
	};
}

#endif  // RAINBOW_OS_IOS || RAINBOW_OS_MACOS
#endif  // CONFUOCO_APPLEAUDIOFILE_H_
