// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <AVFoundation/AVAudioPlayer.h>
#include <AVFoundation/AVAudioSession.h>

#include "FileSystem/Path.h"

namespace
{
	class Stream : public ConFuoco::Sound
	{
	public:
		const ConFuoco::Mixer::Channel *channel;

		Stream(const char *const file, const int loops);
		virtual ~Stream();

		bool is_paused() const;
		bool is_playing() const;
		void set_channel(const ConFuoco::Mixer::Channel *channel);
		void set_volume(const float volume);

		void pause();
		void play();
		void stop();

		operator bool() const;

	private:
		bool paused;
		AVAudioPlayer *player;
	};
}

@interface AudioSession (Private)
- (void)setActive;
- (void)didChangeRoute:(NSDictionary*)notification;
- (void)didInterrupt:(NSDictionary*)notification;
@end

@implementation AudioSession

- (id)init
{
	if (self = [super init])
	{
		[[NSNotificationCenter defaultCenter]
				addObserver:self
				   selector:@selector(didInterrupt:)
				       name:AVAudioSessionInterruptionNotification
				     object:nil];
		[[NSNotificationCenter defaultCenter]
				addObserver:self
				   selector:@selector(didChangeRoute:)
				       name:AVAudioSessionRouteChangeNotification
				     object:nil];
		[self setActive];
	}
	return self;
}

- (void)dealloc
{
	[[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (BOOL)setCategory:(NSString*)category
{
	NSError *error = nil;
	if (!category)
		category = AVAudioSessionCategorySoloAmbient;
	BOOL success = [[AVAudioSession sharedInstance] setCategory:category
	                                                      error:&error];
	if (success && category == AVAudioSessionCategoryPlayAndRecord)
	{
		success = [[AVAudioSession sharedInstance]
				overrideOutputAudioPort:AVAudioSessionPortOverrideSpeaker
				                  error:&error];
		if (!success)
			NSLog(@"%@", error);
	}
	return success;
}

- (void)setActive
{
	NSError *error = nil;
	if (![[AVAudioSession sharedInstance] setActive:YES error:&error])
		NSLog(@"%@", error);
}

- (void)didChangeRoute:(NSNotification*)notification
{
	NSNumber *reason = [notification.userInfo objectForKey:AVAudioSessionRouteChangeReasonKey];
	switch (reason.integerValue)
	{
		case AVAudioSessionRouteChangeReasonUnknown:
			NSLog(@"The reason for the change is unknown.");
			break;
		case AVAudioSessionRouteChangeReasonNewDeviceAvailable:
			NSLog(@"A user action (such as plugging in a headset) has made a preferred audio route available.");
			break;
		case AVAudioSessionRouteChangeReasonOldDeviceUnavailable:
			NSLog(@"The previous audio output path is no longer available.");
			break;
		case AVAudioSessionRouteChangeReasonCategoryChange:
			NSLog(@"The category of the session object changed.");
			break;
		case AVAudioSessionRouteChangeReasonOverride:
			NSLog(@"The output route was overridden by the app.");
			break;
		case AVAudioSessionRouteChangeReasonWakeFromSleep:
			NSLog(@"The route changed when the device woke up from sleep.");
			break;
		case AVAudioSessionRouteChangeReasonNoSuitableRouteForCategory:
			NSLog(@"The route changed because no suitable route is now available for the specified category.");
			break;
		case AVAudioSessionRouteChangeReasonRouteConfigurationChange:
			NSLog(@"The set of input and output ports has not changed, but their configuration has.");
			break;
	}
}

- (void)didInterrupt:(NSNotification*)notification
{
	NSNumber *interuptionType = [notification.userInfo objectForKey:AVAudioSessionInterruptionTypeKey];
	const bool suspend = interuptionType.integerValue == AVAudioSessionInterruptionTypeBegan;
	if (!suspend)
		[self setActive];
	ConFuoco::Mixer::Instance->suspend(suspend);
}

@end

Stream::Stream(const char *const file, const int loops)
    : ConFuoco::Sound(ConFuoco::Sound::Type::Stream), channel(nullptr),
      paused(false), player(nil)
{
	const Path path(file);
	NSError *error = nil;
	this->player = [[AVAudioPlayer alloc] initWithContentsOfURL:path
	                                                      error:&error];
	if (!this->player)
	{
		NSLog(@"%@", error);
		return;
	}
	this->player.numberOfLoops = loops;
	[this->player prepareToPlay];
}

Stream::~Stream()
{
	ConFuoco::Mixer::Instance->release(this);
}

bool Stream::is_paused() const
{
	return this->paused;
}

bool Stream::is_playing() const
{
	return this->player.playing;
}

void Stream::set_volume(const float volume)
{
	this->player.volume = volume;
}

void Stream::pause()
{
	this->paused = true;
	[this->player pause];
}

void Stream::play()
{
	this->paused = false;
	[this->player play];
}

void Stream::stop()
{
	[this->player stop];
	this->paused = false;
	this->player.currentTime = 0;
}

Stream::operator bool() const
{
	return this->player;
}
