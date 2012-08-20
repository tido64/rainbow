// Copyright 2011-12 Bifrost Entertainment. All rights reserved.

#include "Common/Debug.h"
#if defined(RAINBOW_IOS) || defined(RAINBOW_SDL)
#	include "ConFuoco/Mixer.h"

namespace ConFuoco
{
	Engine::~Engine()
	{
		ALCdevice *device = alcGetContextsDevice(this->context);
		alcMakeContextCurrent(0);
		alcDestroyContext(this->context);
		alcCloseDevice(device);
	}

#ifdef RAINBOW_IOS

	void Engine::InterruptionListener(void *client, UInt32 state)
	{
		Engine *engine = static_cast<Engine*>(client);
		switch (state)
		{
			case kAudioSessionBeginInterruption:
				alcMakeContextCurrent(0);
				break;
			case kAudioSessionEndInterruption:
				if (AudioSessionSetActive(true))
					NSLog(@"[Rainbow::ConFuoco] Engine: Failed to activate audio session\n");
				alcMakeContextCurrent(engine->context);
				break;
			default:
				break;
		}
	}

	void Engine::RouteChangeListener(void *, AudioSessionPropertyID, UInt32, const void *data)
	{
		CFDictionaryRef dict = (CFDictionaryRef)data;
		CFStringRef old_route = (CFStringRef)CFDictionaryGetValue(dict, CFSTR(kAudioSession_AudioRouteChangeKey_OldRoute));
		UInt32 size = sizeof(CFStringRef);
		CFStringRef new_route;
		OSStatus result = AudioSessionGetProperty(kAudioSessionProperty_AudioRoute, &size, &new_route);
		NSLog(@"[Rainbow::ConFuoco] Engine: Route changed from %@ to %@ (%ld)\n", old_route, new_route, result);
	}

#endif

	Engine::Engine() : context(nullptr)
	{
	#ifdef RAINBOW_IOS

		OSStatus result = AudioSessionInitialize(0, 0, InterruptionListener, this);
		if (result != 0 && result != kAudioSessionAlreadyInitialized)
		{
			NSLog(@"[Rainbow::ConFuoco] Engine: Failed to initialise audio device (%ld)\n", result);
			return;
		}

		UInt32 property = 0;
		UInt32 propertySize = sizeof(property);
		result = AudioSessionGetProperty(kAudioSessionProperty_OtherAudioIsPlaying, &propertySize, &property);
		if (result != 0)
			NSLog(@"[Rainbow::ConFuoco] Engine: Failed to find out whether audio device is in use (%ld)\n", result);

		property = kAudioSessionCategory_PlayAndRecord;
		result = AudioSessionSetProperty(kAudioSessionProperty_AudioCategory, sizeof(property), &property);
		if (result != 0)
		{
		#if TARGET_IPHONE_SIMULATOR
			property = kAudioSessionCategory_AmbientSound;
			result = AudioSessionSetProperty(kAudioSessionProperty_AudioCategory, sizeof(property), &property);
			if (result != 0)
		#endif
				NSLog(@"[Rainbow::ConFuoco] Engine: Failed to set audio session category (%ld)\n", result);
		}

		property = kAudioSessionOverrideAudioRoute_Speaker;
		result = AudioSessionSetProperty(kAudioSessionProperty_OverrideAudioRoute, sizeof(property), &property);
		if (result != 0)
			NSLog(@"[Rainbow::ConFuoco] Engine: Failed to override audio route (%ld)\n", result);

		result = AudioSessionAddPropertyListener(kAudioSessionProperty_AudioRouteChange, RouteChangeListener, this);
		if (result != 0)
			NSLog(@"[Rainbow::ConFuoco] Engine: Failed to add audio route change listener (%ld)\n", result);

		result = AudioSessionSetActive(true);
		if (result != 0)
			NSLog(@"[Rainbow::ConFuoco] Engine: Failed to activate audio session (%ld)\n", result);

	#endif

		ALCdevice *device = alcOpenDevice(0);
		if (!device)
		{
			R_ERROR("[Rainbow::ConFuoco] Engine: alcOpenDevice error code %x\n", alGetError());
			return;
		}

		this->context = alcCreateContext(device, 0);
		if (!this->context)
		{
			R_ERROR("[Rainbow::ConFuoco] Engine: alcCreateContext error code %x\n", alGetError());
			return;
		}
		alcMakeContextCurrent(this->context);
	}
}

#endif
