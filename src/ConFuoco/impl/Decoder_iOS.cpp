#include "Platform/Definitions.h"
#ifdef RAINBOW_IOS

#include <AudioToolbox/ExtendedAudioFile.h>

#include "ConFuoco/Decoder.h"

namespace ConFuoco
{
	namespace Decoder
	{
		size_t open(char **buffer, int &channels, int &rate, const char *const file)
		{
			NSString *path = [NSString stringWithUTF8String:file];
			path = [[NSBundle mainBundle]
					pathForResource:[path stringByDeletingPathExtension]
					         ofType:[path pathExtension]];
			CFURLRef url = (__bridge CFURLRef)[NSURL fileURLWithPath:path];
			path = nil;

			ExtAudioFileRef ext_audio = 0;
			OSStatus status = ExtAudioFileOpenURL(url, &ext_audio);
			url = nil;
			if (status != noErr)
			{
				NSLog(@"[Rainbow::ConFuoco/iOS] Failed to open %s\n", file);
				return 0;
			}

			AudioStreamBasicDescription audio_format;
			UInt32 size = sizeof(audio_format);
			status = ExtAudioFileGetProperty(ext_audio, kExtAudioFileProperty_FileDataFormat, &size, &audio_format);
			if (status != noErr)
			{
				NSLog(@"[Rainbow::ConFuoco/iOS] Failed to retrieve audio format\n");
				return 0;
			}

			audio_format.mFormatID = kAudioFormatLinearPCM;
			audio_format.mFormatFlags = kAudioFormatFlagIsSignedInteger
			                          | kAudioFormatFlagIsPacked
			                          | kAudioFormatFlagsNativeEndian;
			audio_format.mBytesPerPacket = 2; // * audio_format.mChannelsPerFrame;
			audio_format.mFramesPerPacket = 1;
			audio_format.mBytesPerFrame = audio_format.mBytesPerPacket; // / audio_format.mFramesPerPacket;
			audio_format.mChannelsPerFrame = 1;  // Force mono output
			audio_format.mBitsPerChannel = 16;

			status = ExtAudioFileSetProperty(
					ext_audio, kExtAudioFileProperty_ClientDataFormat,
					sizeof(audio_format), &audio_format);
			if (status != noErr)
			{
				NSLog(@"[Rainbow::ConFuoco/iOS] Failed to set client data format\n");
				return 0;
			}

			SInt64 frames = 0;
			size = sizeof(frames);
			status = ExtAudioFileGetProperty(
					ext_audio, kExtAudioFileProperty_FileLengthFrames, &size, &frames);
			if (status != noErr)
			{
				NSLog(@"[Rainbow::ConFuoco/iOS] Failed to retrieve audio length\n");
				return 0;
			}

			size = frames * audio_format.mBytesPerFrame;
			char *buf = new char[size];

			AudioBufferList abuffer;
			abuffer.mNumberBuffers = 1;
			abuffer.mBuffers[0].mNumberChannels	= audio_format.mChannelsPerFrame;
			abuffer.mBuffers[0].mDataByteSize = size;
			abuffer.mBuffers[0].mData = buf;

			status = ExtAudioFileRead(ext_audio, reinterpret_cast<UInt32*>(&frames), &abuffer);
			if (status != noErr)
			{
				delete[] buf;
				NSLog(@"[Rainbow::ConFuoco/iOS] Failed to read %s\n", file);
				return 0;
			}
			ExtAudioFileDispose(ext_audio);

			*buffer = buf;
			channels = audio_format.mChannelsPerFrame;
			rate = audio_format.mSampleRate;
			return size;
		}
	}
}

#endif
