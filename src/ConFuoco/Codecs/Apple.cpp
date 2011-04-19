#include "../Decoder.h"

#ifdef RAINBOW_IOS

#include <AudioToolbox/AudioToolbox.h>
#include <AudioToolbox/ExtendedAudioFile.h>

namespace Rainbow
{
	namespace ConFuoco
	{
		Decoder::Decoder() { }

		void Decoder::open(Wave &wave, const char *media)
		{
			CFURLRef url = CFURLCreateFromFileSystemRepresentation(0, reinterpret_cast<const UInt8 *>(media), strlen(media), false);
			ExtAudioFileRef ext_audio = 0;
			if (ExtAudioFileOpenURL(url, &ext_audio) != noErr)
			{
				CFRelease(url);
				NSLog(@"Rainbow::Audio::Decoder: Could not open %s\n", CFURLGetString(url));
				return;
			}
			CFRelease(url);

			AudioStreamBasicDescription audio_format;
			UInt32 size = sizeof(audio_format);
			if (ExtAudioFileGetProperty(ext_audio, kExtAudioFileProperty_FileDataFormat, &size, &audio_format) != noErr)
			{
				NSLog(@"Rainbow::Audio::Decoder: Could not get audio format\n");
				return;
			}
			assert(audio_format.mChannelsPerFrame <= 2);

			audio_format.mFormatID = kAudioFormatLinearPCM;
			audio_format.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked | kAudioFormatFlagsNativeEndian;
			audio_format.mBytesPerPacket = 2; // * audio_format.mChannelsPerFrame;
			audio_format.mFramesPerPacket = 1;
			audio_format.mBytesPerFrame = audio_format.mBytesPerPacket / audio_format.mFramesPerPacket;
			audio_format.mChannelsPerFrame = 1;  // Force mono output
			audio_format.mBitsPerChannel = 16;

			if (ExtAudioFileSetProperty(ext_audio, kExtAudioFileProperty_ClientDataFormat, sizeof(audio_format), &audio_format) != noErr)
			{
				NSLog(@"Rainbow::Audio::Decoder: Could not set client data format\n");
				return;
			}

			SInt64 frames = 0;
			size = sizeof(frames);
			if (ExtAudioFileGetProperty(ext_audio, kExtAudioFileProperty_FileLengthFrames, &size, &frames) != noErr)
			{
				NSLog(@"Rainbow::Audio::Decoder: Could not get audio length\n");
				return;
			}

			size = frames * audio_format.mBytesPerFrame;
			wave.buffer = new byte[size];
			assert(wave.buffer != 0);
			memset(wave.buffer, 0, size);

			AudioBufferList abuffer;
			abuffer.mNumberBuffers = 1;
			abuffer.mBuffers[0].mNumberChannels	= audio_format.mChannelsPerFrame;
			abuffer.mBuffers[0].mDataByteSize = size;
			abuffer.mBuffers[0].mData = wave.buffer;

			if (ExtAudioFileRead(ext_audio, reinterpret_cast<UInt32 *>(&frames), &abuffer) != noErr)
			{
				delete[] wave.buffer;
				wave.buffer = 0;
				NSLog(@"Rainbow::Audio::Decoder: Could not read from %s\n", media);
				return;
			}
			ExtAudioFileDispose(ext_audio);

			wave.buffer_size = size;
			wave.format = AL_FORMAT_MONO16; //(audio_format.mChannelsPerFrame > 1) ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;
			wave.frequency = audio_format.mSampleRate;
		}
	}
}

#endif
