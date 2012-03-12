// Copyright 2012 Bifrost Entertainment. All rights reserved.

#include "ConFuoco/Recorder.h"
#ifdef RAINBOW_IOS

#include <CoreAudio/CoreAudioTypes.h>

namespace ConFuoco
{
	Recorder::Recorder() :
		average(0.0f), low_pass(0.0f), peak(0.0f), recorder(nullptr)
	{
		NSURL *dev_null = [NSURL fileURLWithPath:@"/dev/null"];
		NSDictionary *settings = [NSDictionary dictionaryWithObjectsAndKeys:
			[NSNumber numberWithInt: kAudioFormatAppleLossless], AVFormatIDKey,
			[NSNumber numberWithFloat: 22050.0],                 AVSampleRateKey,
			[NSNumber numberWithInt: 1],                         AVNumberOfChannelsKey,
			[NSNumber numberWithInt: AVAudioQualityHigh],        AVEncoderAudioQualityKey,
			nil];
		NSError *error;
		this->recorder = [[AVAudioRecorder alloc] initWithURL:dev_null settings:settings error:&error];

		if (!this->recorder)
			NSLog(@"%@", [error description]);
		else
		{
			[recorder setMeteringEnabled:YES];
			[recorder prepareToRecord];
		}
	}

	Recorder::~Recorder() { }

	void Recorder::update()
	{
		[recorder updateMeters];

		const float alpha = 0.05f;
		this->average = [recorder averagePowerForChannel:0];
		this->peak = [recorder peakPowerForChannel:0];
		this->low_pass = alpha * powf(10.0f, this->average * alpha) + (1.0f - alpha) * this->low_pass;
	}
}

#else

namespace ConFuoco
{
	Recorder::Recorder() :
		average(0.0f), low_pass(0.0f), peak(0.0f), recorder(nullptr) { }

	Recorder::~Recorder() { }
	void Recorder::update() { }
}

#endif
