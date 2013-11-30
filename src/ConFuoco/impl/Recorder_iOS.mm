// Copyright 2012-13 Bifrost Entertainment. All rights reserved.

#include "ConFuoco/Recorder.h"
#ifdef RAINBOW_IOS

#include <AVFoundation/AVAudioRecorder.h>
#include <AVFoundation/AVAudioSession.h>
#include <CoreAudio/CoreAudioTypes.h>

#include "Common/Algorithm.h"

#define kTemporaryFilePath \
	[NSTemporaryDirectory() stringByAppendingPathComponent:@"com.bifrostentertainment.maja"]

namespace ConFuoco
{
	Recorder::Recorder() :
		average(0.0f), low_pass(0.0f), peak(0.0f), recorder(nullptr)
	{
		NSString *path = kTemporaryFilePath;
		if ([[NSFileManager defaultManager] fileExistsAtPath:path])
		{
			NSError *error;
			[[NSFileManager defaultManager] removeItemAtPath:path error:&error];
		}
		NSDictionary *settings = [NSDictionary dictionaryWithObjectsAndKeys:
			[NSNumber numberWithInt:kAudioFormatLinearPCM],  AVFormatIDKey,
			[NSNumber numberWithFloat:44100.0],              AVSampleRateKey,
			[NSNumber numberWithInt:1],                      AVNumberOfChannelsKey,
			[NSNumber numberWithInt:16],                     AVLinearPCMBitDepthKey,
			[NSNumber numberWithInt:AVAudioQualityHigh],     AVEncoderAudioQualityKey,
			nil];
		NSError *error;
		this->recorder = [[AVAudioRecorder alloc]
				initWithURL:[NSURL fileURLWithPath:path isDirectory:NO]
				   settings:settings
				      error:&error];
		if (!this->recorder)
		{
			NSLog(@"%@", [error description]);
			return;
		}
		[recorder setMeteringEnabled:YES];
		[recorder prepareToRecord];
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
		NSError *error;
		[[NSFileManager defaultManager] removeItemAtPath:kTemporaryFilePath
		                                           error:&error];
	}

	void Recorder::update()
	{
		if (!recorder)
			return;

		NSError *error;
		NSDictionary *attributes = [[NSFileManager defaultManager]
				attributesOfItemAtPath:kTemporaryFilePath
				                 error:&error];
		if (attributes.fileSize >= 8 * 1024 * 1024)
		{
			this->stop();
			this->record();
			return;
		}

		[recorder updateMeters];
		this->average = [recorder averagePowerForChannel:0];
		this->peak = [recorder peakPowerForChannel:0];
		this->low_pass = Rainbow::low_pass(this->peak, this->low_pass);
	}
}

#endif
