// Copyright 2012-13 Bifrost Entertainment. All rights reserved.

#include "ConFuoco/Recorder.h"
#ifdef RAINBOW_IOS

#include <AVFoundation/AVAudioRecorder.h>
#include <CoreAudio/CoreAudioTypes.h>

#include "Common/Algorithm.h"

namespace ConFuoco
{
	Recorder::Recorder() :
		average(0.0f), low_pass(0.0f), peak(0.0f), recorder(nullptr)
	{
		NSURL *dev_null = [NSURL fileURLWithPath:@"/dev/null"];
		NSDictionary *settings = [NSDictionary dictionaryWithObjectsAndKeys:
			[NSNumber numberWithInt: kAudioFormatLinearPCM],  AVFormatIDKey,
			[NSNumber numberWithFloat: 44100.0],              AVSampleRateKey,
			[NSNumber numberWithInt: 1],                      AVNumberOfChannelsKey,
			[NSNumber numberWithInt: 16],                     AVLinearPCMBitDepthKey,
			[NSNumber numberWithInt: AVAudioQualityHigh],     AVEncoderAudioQualityKey,
			nil];
		NSError *error;
		this->recorder = [[AVAudioRecorder alloc] initWithURL:dev_null
		                                             settings:settings
		                                                error:&error];
		if (!this->recorder)
			NSLog(@"%@", [error description]);
		else
		{
			[recorder setMeteringEnabled:YES];
			[recorder prepareToRecord];
		}
	}

	Recorder::~Recorder() { }

	void Recorder::pause()
	{
		[recorder pause];
	}

	bool Recorder::record(const unsigned long duration)
	{
		return duration ? [recorder recordForDuration:duration] : [recorder record];
	}

	void Recorder::stop()
	{
		[recorder stop];
	}

	void Recorder::update()
	{
		[recorder updateMeters];

		this->average = [recorder averagePowerForChannel:0];
		this->peak = [recorder peakPowerForChannel:0];

		this->low_pass = Rainbow::low_pass(this->peak, this->low_pass);
	}
}

#endif
