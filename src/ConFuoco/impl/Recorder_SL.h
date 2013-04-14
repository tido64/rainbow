#ifndef CONFUOCO_RECORDER_SL_H_
#define CONFUOCO_RECORDER_SL_H_

#ifdef RAINBOW_ANDROID

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

namespace ConFuoco
{
	static const size_t kInputSamples = 512;
	static const size_t kNumInputSampleBuffers = 2;

	struct SLRecorder
	{
		size_t active_buffer;
		SLObjectItf object;
		SLRecordItf interface;
		SLAndroidSimpleBufferQueueItf buffer_queue;
		short *buffer;
		short *samples;

		SLRecorder(SLEngineItf engine);
		~SLRecorder();
		void set_callback(slAndroidSimpleBufferQueueCallback callback, void *context);
		inline operator bool() const;
	};

	SLRecorder::operator bool() const
	{
		return this->buffer;
	}
}

#endif  // RAINBOW_ANDROID
#endif  // CONFUOCO_RECORDER_SL_H_
