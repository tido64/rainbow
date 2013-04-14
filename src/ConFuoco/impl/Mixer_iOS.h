#ifdef RAINBOW_IOS
#include <AudioToolbox/AudioServices.h>
#include <AVFoundation/AVAudioPlayer.h>

namespace ConFuoco
{
	namespace
	{
		struct Stream : public Sound
		{
			bool paused;
			AVAudioPlayer *player;
			Channel *channel;

			Stream(Mixer *m, const char *const filename, const int loops) :
				Sound(STREAM, m), paused(false), player(nil), channel(nullptr)
			{
				NSString *file = [NSString stringWithUTF8String:filename];
				NSString *path = [[NSBundle mainBundle]
						pathForResource:[file stringByDeletingPathExtension]
						         ofType:[file pathExtension]];
				if (!path)
				{
					NSLog(@"[Rainbow::ConFuoco/iOS] Failed to locate '%@'", file);
					return;
				}
				NSError *err = nil;
				this->player = [[AVAudioPlayer alloc]
						initWithContentsOfURL:[NSURL fileURLWithPath:path]
						                error:&err];
				if (!this->player)
				{
					NSLog(@"[Rainbow::ConFuoco/iOS] %@", [err description]);
					return;
				}
				this->player.numberOfLoops = loops;
				[this->player prepareToPlay];
			}

			virtual ~Stream()
			{
				this->mixer->release(this);
				this->player = nil;
			}

			void pause()
			{
				[this->player pause];
				this->paused = true;
			}

			void play()
			{
				[this->player play];
				this->paused = false;
			}

			void stop()
			{
				[this->player stop];
				this->paused = false;
				this->player.currentTime = 0;
			}

			operator bool() const
			{
				return this->player;
			}
		};

		void InterruptionListener(void *client, UInt32 state)
		{
			Mixer *mixer = static_cast<Mixer*>(client);
			switch (state)
			{
				case kAudioSessionBeginInterruption:
					mixer->suspend(true);
					break;
				case kAudioSessionEndInterruption:
					if (AudioSessionSetActive(true))
						NSLog(@"[Rainbow::ConFuoco/iOS] Failed to activate audio session\n");
					mixer->suspend(false);
					break;
				default:
					break;
			}
		}

		void RouteChangeListener(void *, AudioSessionPropertyID, UInt32, const void *data)
		{
			CFDictionaryRef dict = (CFDictionaryRef)data;
			CFStringRef old_route = (CFStringRef)CFDictionaryGetValue(
					dict, CFSTR(kAudioSession_AudioRouteChangeKey_OldRoute));
			UInt32 size = sizeof(CFStringRef);
			CFStringRef new_route;
			OSStatus result = AudioSessionGetProperty(
					kAudioSessionProperty_AudioRoute, &size, &new_route);
			NSLog(@"[Rainbow::ConFuoco/iOS] Route changed from %@ to %@ (%ld)\n",
			      old_route, new_route, result);
		}

		void InitAudioSession(Mixer *mixer)
		{
			OSStatus result = AudioSessionInitialize(0, 0, InterruptionListener, mixer);
			if (result != 0 && result != kAudioSessionAlreadyInitialized)
			{
				NSLog(@"[Rainbow::ConFuoco/iOS] Failed to initialise audio device (%ld)\n", result);
				return;
			}

			UInt32 property = 0;
			UInt32 propertySize = sizeof(property);
			result = AudioSessionGetProperty(
					kAudioSessionProperty_OtherAudioIsPlaying, &propertySize, &property);
			if (result != 0)
				NSLog(@"[Rainbow::ConFuoco/iOS] Failed to find out whether audio device is in use (%ld)\n", result);

			property = kAudioSessionCategory_PlayAndRecord;
			result = AudioSessionSetProperty(
					kAudioSessionProperty_AudioCategory, sizeof(property), &property);
			if (result != 0)
			{
			#if TARGET_IPHONE_SIMULATOR
				property = kAudioSessionCategory_AmbientSound;
				result = AudioSessionSetProperty(
						kAudioSessionProperty_AudioCategory, sizeof(property), &property);
				if (result != 0)
			#endif
					NSLog(@"[Rainbow::ConFuoco/iOS] Failed to set audio session category (%ld)\n", result);
			}

			property = kAudioSessionOverrideAudioRoute_Speaker;
			result = AudioSessionSetProperty(
					kAudioSessionProperty_OverrideAudioRoute, sizeof(property), &property);
			if (result != 0)
				NSLog(@"[Rainbow::ConFuoco/iOS] Failed to override audio route (%ld)\n", result);

			result = AudioSessionAddPropertyListener(
					kAudioSessionProperty_AudioRouteChange, RouteChangeListener, mixer);
			if (result != 0)
				NSLog(@"[Rainbow::ConFuoco/iOS] Failed to add audio route change listener (%ld)\n", result);

			result = AudioSessionSetActive(true);
			if (result != 0)
				NSLog(@"[Rainbow::ConFuoco/iOS] Failed to activate audio session (%ld)\n", result);
		}
	}
}

#endif
