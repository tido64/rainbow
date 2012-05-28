// Copyright 2011-12 Bifrost Entertainment. All rights reserved.

#include "ConFuoco/Mixer.h"
#if defined(RAINBOW_IOS) || defined(RAINBOW_SDL)

#include <cstdio>

#include "ConFuoco/Sound.h"
#include "ConFuoco/Stream.h"

namespace ConFuoco
{
	Mixer::~Mixer()
	{
		this->clear();
		ALCdevice *device = alcGetContextsDevice(this->context);
		alcMakeContextCurrent(0);
		alcDestroyContext(this->context);
		alcCloseDevice(device);
	}

	Wave* Mixer::load_sfx(const char *const file, const unsigned int instances)
	{
		Sound *s = this->bank.create_sound(instances);
		s->load(file);
		return s;
	}

	Wave* Mixer::load_stream(const char *const file)
	{
		Stream *s = this->bank.create_stream();
		s->load(file);
		return s;
	}

#ifdef RAINBOW_IOS

	void Mixer::InterruptionListener(void *client, UInt32 state)
	{
		Mixer *mixer = static_cast<Mixer *>(client);
		switch (state)
		{
			case kAudioSessionBeginInterruption:
				alcMakeContextCurrent(0);
				break;
			case kAudioSessionEndInterruption:
				if (AudioSessionSetActive(true))
					NSLog(@"Rainbow::ConFuoco::iOS::Mixer: Failed to activate audio session\n");
				alcMakeContextCurrent(mixer->context);
				break;
			default:
				break;
		}
	}

	void Mixer::RouteChangeListener(void *, AudioSessionPropertyID, UInt32, const void *data)
	{
		CFDictionaryRef dict = (CFDictionaryRef)data;
		CFStringRef old_route = (CFStringRef)CFDictionaryGetValue(dict, CFSTR(kAudioSession_AudioRouteChangeKey_OldRoute));
		UInt32 size = sizeof(CFStringRef);
		CFStringRef new_route;
		OSStatus result = AudioSessionGetProperty(kAudioSessionProperty_AudioRoute, &size, &new_route);
		NSLog(@"Rainbow::ConFuoco::iOS::Mixer: Route changed from %@ to %@ (%ld)\n", old_route, new_route, result);
	}

#endif

	Mixer::Mixer() : context(nullptr)
	{
	#ifdef RAINBOW_IOS

		OSStatus result = AudioSessionInitialize(0, 0, InterruptionListener, this);
		if (result != 0 && result != kAudioSessionAlreadyInitialized)
		{
			NSLog(@"Rainbow::ConFuoco::iOS::Mixer: Failed to initialise audio device (%ld)\n", result);
			return;
		}

		UInt32 property = 0;
		UInt32 propertySize = sizeof(property);
		result = AudioSessionGetProperty(kAudioSessionProperty_OtherAudioIsPlaying, &propertySize, &property);
		if (result != 0)
			NSLog(@"Rainbow::ConFuoco::iOS::Mixer: Failed to find out whether audio device is in use (%ld)\n", result);

		property = kAudioSessionCategory_PlayAndRecord;
		result = AudioSessionSetProperty(kAudioSessionProperty_AudioCategory, sizeof(property), &property);
		if (result != 0)
		{
		#if TARGET_IPHONE_SIMULATOR
			property = kAudioSessionCategory_AmbientSound;
			result = AudioSessionSetProperty(kAudioSessionProperty_AudioCategory, sizeof(property), &property);
			if (result != 0)
		#endif
				NSLog(@"Rainbow::ConFuoco::iOS::Mixer: Failed to set audio session category (%ld)\n", result);
		}

		property = kAudioSessionOverrideAudioRoute_Speaker;
		result = AudioSessionSetProperty(kAudioSessionProperty_OverrideAudioRoute, sizeof(property), &property);
		if (result != 0)
			NSLog(@"Rainbow::ConFuoco::iOS::Mixer: Failed to override audio route (%ld)\n", result);

		result = AudioSessionAddPropertyListener(kAudioSessionProperty_AudioRouteChange, RouteChangeListener, this);
		if (result != 0)
			NSLog(@"Rainbow::ConFuoco::iOS::Mixer: Failed to add audio route change listener (%ld)\n", result);

		result = AudioSessionSetActive(true);
		if (result != 0)
			NSLog(@"Rainbow::ConFuoco::iOS::Mixer: Failed to activate audio session (%ld)\n", result);

	#endif

		ALCdevice *device = alcOpenDevice(0);
		if (!device)
		{
			fprintf(stderr, "Rainbow::ConFuoco::AL::Mixer: alcOpenDevice error code %x\n", alGetError());
			return;
		}

		this->context = alcCreateContext(device, 0);
		if (!this->context)
		{
			fprintf(stderr, "Rainbow::ConFuoco::AL::Mixer: alcCreateContext error code %x\n", alGetError());
			return;
		}
		alcMakeContextCurrent(this->context);
	}
}

#endif
