#ifndef CONFUOCO_IMPL_RECORDER_SL_H_
#define CONFUOCO_IMPL_RECORDER_SL_H_

#ifdef RAINBOW_OS_ANDROID

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

namespace ConFuoco
{
	struct SLRecorder
	{
		size_t active_buffer;
		SLObjectItf object;
		SLRecordItf interface;
		SLAndroidSimpleBufferQueueItf buffer_queue;
		short *buffer;
		short *samples;

		SLRecorder();
		~SLRecorder();
		void set_callback(slAndroidSimpleBufferQueueCallback callback, void *context);
		inline operator bool() const;
	};

	SLRecorder::operator bool() const
	{
		return this->buffer;
	}
}

#endif  // RAINBOW_OS_ANDROID
#endif  // CONFUOCO_RECORDER_SL_H_
