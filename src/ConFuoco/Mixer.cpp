/*
 *  Mixer.cpp
 *  OnWire
 *
 *  Created by Tommy Nguyen on 3/29/11.
 *  Copyright 2011 Bifrost Games. All rights reserved.
 *
 */

#include "Mixer.h"
#include "Decoder.h"

namespace Rainbow
{
	namespace ConFuoco
	{
		Mixer::Mixer() :
			interrupted(false), playing(false), cleanup(0), busy(0),
			buffer_count(0), source_count(0), context(0)
		{
			#ifdef RAINBOW_IOS
				this->init_audio_session();
			#endif

			this->init_AL();
		}

		Mixer::~Mixer()
		{
			this->clear();

			if (this->context)
			{
				if (this->cleanup & AUDIO_SOURCED)
				{
					#ifndef RAINBOW_IOS
						alDeleteSources(1, &this->stream.sourced);
					#endif

					alDeleteSources(AUDIO_SOURCES, this->sources);
				}
				if (this->cleanup & AUDIO_BUFFERED)
				{
					#ifndef RAINBOW_IOS
						alDeleteBuffers(STREAM_BUFFERS, this->stream.buffered);
					#endif

					alDeleteBuffers(AUDIO_BUFFERS, this->buffers);
				}
				ALCdevice *device = alcGetContextsDevice(context);
				alcMakeContextCurrent(0);
				alcDestroyContext(context);
				alcCloseDevice(device);
			}
		}

		void Mixer::clear()
		{
			if (this->context)
			{
				for (unsigned int i = 0; i < this->source_count; ++i)
					alSourceStop(this->sources[i]);
				this->source_count = 0;

				for (unsigned int i = 0; i < this->buffer_count; ++i)
					this->waves[i].release();
				this->buffer_count = 0;
			}
			this->stop_bgm(true);
		}

		void Mixer::update()
		{
			int work = 0;

			// Move all playing sources to the front
			for (unsigned int i = 0; i < this->source_count; ++i)
			{
				alGetSourcei(this->sources[i], AL_SOURCE_STATE, &work);
				if (work != AL_PLAYING)
				{
					unsigned int j = this->source_count - 1;
					for (; j > i; --j)
					{
						alGetSourcei(this->sources[j], AL_SOURCE_STATE, &work);
						if (work == AL_PLAYING)
							break;
					}
					if (work != AL_PLAYING)
					{
						this->source_count = i;
						break;
					}
					work = this->sources[i];
					this->sources[i] = this->sources[j];
					this->sources[j] = work;
					this->source_count = j - 1;
				}
			}

		#ifndef RAINBOW_IOS

			if (this->interrupted)
				return;

			alGetSourcei(this->stream.sourced, AL_SOURCE_STATE, &work);
			if(work != AL_PLAYING)
				return;

			unsigned int buffer = 0;
			alGetSourcei(this->stream.sourced, AL_BUFFERS_PROCESSED, &work);
			for (int i = 0; i < work; ++i)
			{
				printf("%i buffer(s) processed\n", work);
				int packets = Decoder::Instance().read(this->stream);
				printf("Read additional %i packet(s)\n", packets);
				if (!packets)
				{
					puts("Finished");
					exit(0);
					Decoder::Instance().reset(this->stream);
					Decoder::Instance().read(this->stream);
				}
				alSourceUnqueueBuffers(this->stream.sourced, 1, &buffer);
				alBufferData(buffer, this->stream.format, this->stream.buffer, this->stream.buffer_size, this->stream.frequency);
				alSourceQueueBuffers(this->stream.sourced, 1, &buffer);
			}
			alGetSourcei(this->stream.sourced, AL_SOURCE_STATE, &work);
			if(work != AL_PLAYING)
				alSourcePlay(this->stream.sourced);

		#endif
		}

		unsigned int Mixer::add(const char *media)
		{
			Wave &w = waves[this->buffer_count];
			w.buffered = this->buffer_count++;
			Decoder::Instance().open(w, media);
			alBufferData(this->buffers[w.buffered], w.format, w.buffer, w.buffer_size, w.frequency);
			return w.buffered;
		}

		unsigned int Mixer::play(const unsigned int id, const float x, const float y, const float z)
		{
			const unsigned int src = this->sources[this->source_count++];
			alSourcei(src, AL_BUFFER, this->buffers[id]);
			alSourcePlay(src);
			return src;
		}

		void Mixer::set_bgm(const char *media)
		{
			this->stop_bgm(true);

		#ifdef RAINBOW_IOS

			NSURL *fileURL = [[NSURL alloc] initFileURLWithPath:[NSString stringWithUTF8String:media]];
			this->player = [[AVAudioPlayer alloc] initWithContentsOfURL:fileURL error:nil];
			[fileURL release];
			this->player.numberOfLoops = -1;
			[this->player prepareToPlay];

		#else

			Decoder::Instance().open_stream(this->stream, media);
			for (unsigned int i = 0; i < STREAM_BUFFERS; ++i)
			{
				Decoder::Instance().read(this->stream);
				alBufferData(this->stream.buffered[i], this->stream.format, this->stream.buffer, this->stream.buffer_size, this->stream.frequency);
			}
			alSourceQueueBuffers(this->stream.sourced, STREAM_BUFFERS, this->stream.buffered);

		#endif
		}

		void Mixer::play_bgm()
		{
		#ifdef RAINBOW_IOS

			if (!this->player || this->player.playing)
				return;

			[this->player play];

		#else

			if (!this->stream.in_use)
				return;

			int state = 0;
			alGetSourcei(this->stream.sourced, AL_SOURCE_STATE, &state);
			if (state == AL_PLAYING)
				return;

			alSourcePlay(this->stream.sourced);

		#endif
		}

		void Mixer::pause_bgm()
		{
		#ifdef RAINBOW_IOS

			if (this->player.playing)
				[this->player pause];

		#else

			int state = 0;
			alGetSourcei(this->stream.sourced, AL_SOURCE_STATE, &state);
			if (state == AL_PLAYING)
				alSourcePause(this->stream.sourced);

		#endif
		}

		void Mixer::stop_bgm(bool clear)
		{
		#ifdef RAINBOW_IOS

			if (this->player)
			{
				if (this->player.playing)
					[this->player stop];
				if (clear)
				{
					[this->player release];
					this->player = 0;
				}
			}

		#else

			if (this->stream.in_use)
			{
				int state = 0;
				alGetSourcei(this->stream.sourced, AL_SOURCE_STATE, &state);
				if (state == AL_PLAYING)
					alSourceStop(this->stream.sourced);
				if (clear)
					this->stream.release();
			}

		#endif
		}

	#ifdef RAINBOW_IOS

		void Mixer::set_bgm_pan(float p)
		{
			if (p > 1.0f)
				p = 1.0f;
			else if (p < -1.0f)
				p = -1.0f;
			this->player.pan = p;
		}

		void Mixer::set_bgm_volume(float v)
		{
			if (v > 1.0f)
				v = 1.0f;
			else if (v < 0.0f)
				v = 0.0f;
			this->player.volume = v;
		}

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
						NSLog(@"Rainbow::Audio::Mixer: Could not activate audio session\n");
					alcMakeContextCurrent(mixer->context);
					break;
				default:
					break;
			}
		}

		void Mixer::RouteChangeListener(void *client, AudioSessionPropertyID id, UInt32 data_size, const void *data)
		{
			CFDictionaryRef dict = (CFDictionaryRef)data;
			CFStringRef old_route = (CFStringRef)CFDictionaryGetValue(dict, CFSTR(kAudioSession_AudioRouteChangeKey_OldRoute));
			UInt32 size = sizeof(CFStringRef);
			CFStringRef new_route;
			OSStatus result = AudioSessionGetProperty(kAudioSessionProperty_AudioRoute, &size, &new_route);
			NSLog(@"Rainbow::Audio::Mixer: Route changed from %@ to %@ (%d)\n", old_route, new_route, result);
		}

		void Mixer::init_audio_session()
		{
			this->player = 0;

			OSStatus result = AudioSessionInitialize(0, 0, InterruptionListener, this);
			if (result != 0 && result != kAudioSessionAlreadyInitialized)
			{
				NSLog(@"Rainbow::Audio::Mixer: Could not initialise audio device (%d)\n", result);
				return;
			}

			UInt32 property = sizeof(this->busy);
			result = AudioSessionGetProperty(kAudioSessionProperty_OtherAudioIsPlaying, &property, &this->busy);
			if (result != 0)
				NSLog(@"Rainbow::Audio::Mixer: Could not find out whether audio device is in use (%d)\n", result);

			property = (this->busy) ? kAudioSessionCategory_AmbientSound : kAudioSessionCategory_SoloAmbientSound;
			result = AudioSessionSetProperty(kAudioSessionProperty_AudioCategory, sizeof(property), &property);
			if (result != 0)
			{
				// This is an iOS simulator workaround
				property = kAudioSessionCategory_AmbientSound;
				result = AudioSessionSetProperty(kAudioSessionProperty_AudioCategory, sizeof(property), &property);
				if (result != 0)
					NSLog(@"Rainbow::Audio::Mixer: Could not set audio session category (%d)\n", result);
			}

			result = AudioSessionAddPropertyListener(kAudioSessionProperty_AudioRouteChange, RouteChangeListener, this);
			if (result != 0)
				NSLog(@"Rainbow::Audio::Mixer: Could not add audio route change listener (%d)\n", result);

			result = AudioSessionSetActive(true);
			if (result != 0)
				NSLog(@"Rainbow::Audio::Mixer: Could not activate audio session (%s)\n", result);
		}

	#endif

		void Mixer::init_AL()
		{
			ALCdevice *device = alcOpenDevice(0);
			if (device == 0)
			{
				alGetError();
				return;
			}

			this->context = alcCreateContext(device, 0);
			if (this->context == 0)
			{
				alGetError();
				return;
			}

			alcMakeContextCurrent(this->context);
			ALenum err;

			#ifndef RAINBOW_IOS
				alGenBuffers(STREAM_BUFFERS, this->stream.buffered);
			#endif
			alGenBuffers(AUDIO_BUFFERS, this->buffers);
			if ((err = alGetError()) != AL_NO_ERROR)
			{
				printf("Rainbow::Audio::Mixer: Could not generate buffers (%x)\n", err);
				exit(1);
			}
			this->cleanup |= AUDIO_BUFFERED;

			#ifndef RAINBOW_IOS
				alGenSources(1, &this->stream.sourced);
			#endif
			alGenSources(AUDIO_SOURCES, this->sources);
			if ((err = alGetError()) != AL_NO_ERROR)
			{
				printf("Rainbow::Audio::Mixer: Could not generate sources (%x)\n", err);
				exit(1);
			}
			this->cleanup |= AUDIO_SOURCED;

			#ifndef RAINBOW_IOS
				alSourcei(this->stream.sourced, AL_SOURCE_RELATIVE, AL_TRUE);
				alSourcei(this->stream.sourced, AL_ROLLOFF_FACTOR, 0);
			#endif
			for (unsigned int i = 0; i < AUDIO_SOURCES; ++i)
			{
				alSourcei(this->sources[i], AL_SOURCE_RELATIVE, AL_TRUE);
				alSourcei(this->sources[i], AL_ROLLOFF_FACTOR, 0);
				if ((err = alGetError()) != AL_NO_ERROR)
				{
					printf("Rainbow::Audio::Mixer: Could not set source parameters (%x)\n", err);
					exit(1);
				}
			}
		}
	}
}
