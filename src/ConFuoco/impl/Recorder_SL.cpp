// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "ConFuoco/Recorder.h"
#if defined(RAINBOW_OS_ANDROID) && !defined(USE_FMOD_STUDIO)

#include <climits>
#include <cmath>

#include "Common/Algorithm.h"
#include "ConFuoco/impl/Mixer_SL.h"

#define CF_TAG "[Rainbow::ConFuoco/SL] "

#define slClear(self)                         (*self)->Clear(self)
#define slCreateAudioRecorder(self, recorder, source, sink, interfaces, iids, req) \
	(*self)->CreateAudioRecorder(self, recorder, source, sink, interfaces, iids, req)
#define slDestroy(self)                       (*self)->Destroy(self)
#define slEnqueue(self, buffer, size)         (*self)->Enqueue(self, buffer, size)
#define slGetInterface(self, iid, interface)  (*self)->GetInterface(self, iid, interface)
#define slGetRecordState(self, state)         (*self)->GetRecordState(self, state)
#define slRealize(self, async)                (*self)->Realize(self, async)
#define slRegisterCallback(self, callback, context) \
	(*self)->RegisterCallback(self, callback, context)
#define slSetDurationLimit(self, msec)        (*self)->SetDurationLimit(self, msec)
#define slSetRecordState(self, state)         (*self)->SetRecordState(self, state)

namespace ConFuoco
{
	namespace
	{
		const float kNormalizeSample = 1.0f / SHRT_MAX;

		float decibels(const float amplitude)
		{
			return 20 * log10f(amplitude);
		}

		void AndroidSimpleBufferQueueCallback(SLAndroidSimpleBufferQueueItf, void *pContext)
		{
			static_cast<RecorderSL*>(pContext)->swap();
		}
	}

	RecorderSL::RecorderSL()
	    : current(kNumInputSampleBuffers), object(nullptr), interface(nullptr),
	      buffer_queue(nullptr)
	{
		SLDataLocator_IODevice iodev = {
			SL_DATALOCATOR_IODEVICE,
			SL_IODEVICE_AUDIOINPUT,
			SL_DEFAULTDEVICEID_AUDIOINPUT,
			nullptr
		};
		SLDataSource source = { &iodev, nullptr };

		SLDataLocator_AndroidSimpleBufferQueue buffer_queue = {
			SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, kNumInputSampleBuffers
		};
		SLDataFormat_PCM pcm_format = {
			SL_DATAFORMAT_PCM,
			1,
			SL_SAMPLINGRATE_16,
			SL_PCMSAMPLEFORMAT_FIXED_16,
			SL_PCMSAMPLEFORMAT_FIXED_16,
			SL_SPEAKER_FRONT_CENTER,
			SL_BYTEORDER_LITTLEENDIAN
		};
		SLDataSink sink = { &buffer_queue, &pcm_format };

		const SLInterfaceID iids[] = { SL_IID_ANDROIDSIMPLEBUFFERQUEUE };
		const SLboolean req[] = { SL_BOOLEAN_TRUE };

		SLEngineItf engine = ConFuoco::Mixer::Instance->interface();
		if (slCreateAudioRecorder(engine, &this->object, &source, &sink, 1, iids, req) != SL_RESULT_SUCCESS)
		{
			LOGE("ConFuoco/SL: Failed to create audio recorder");
			return;
		}
		if (slRealize(this->object, SL_BOOLEAN_FALSE) != SL_RESULT_SUCCESS)
		{
			LOGE("ConFuoco/SL: Failed to realize audio recorder");
			return;
		}
		if (slGetInterface(this->object, SL_IID_RECORD, &this->interface) != SL_RESULT_SUCCESS)
		{
			LOGE("ConFuoco/SL: Failed to get audio recorder interface");
			return;
		}
		if (slGetInterface(this->object, SL_IID_ANDROIDSIMPLEBUFFERQUEUE, &this->buffer_queue) != SL_RESULT_SUCCESS)
		{
			LOGE("ConFuoco/SL: Failed to get buffer queue interface");
			return;
		}
		if (slRegisterCallback(this->buffer_queue, AndroidSimpleBufferQueueCallback, this) != SL_RESULT_SUCCESS)
		{
			LOGE("ConFuoco/SL: Failed to set audio recorder callback");
			return;
		}

		memset(this->buffer, 0, sizeof(this->buffer));
		this->current = 0;
	}

	RecorderSL::~RecorderSL()
	{
		if (this->object)
		{
			if (this->current < kNumInputSampleBuffers)
				this->stop();
			slDestroy(this->object);
		}
	}

	void RecorderSL::swap()
	{
		this->current = this->next();
		slEnqueue(this->buffer_queue, this->get_buffer(this->next()), kInputSamples * sizeof(short));
	}

	short* RecorderSL::get_buffer(const unsigned int i)
	{
		return this->buffer + kInputSamples * i;
	}

	unsigned int RecorderSL::next() const
	{
		return (this->current + 1) % kNumInputSampleBuffers;
	}

	void RecorderSL::pause_impl()
	{
		if (this->current == kNumInputSampleBuffers)
			return;

		slSetRecordState(this->interface, SL_RECORDSTATE_PAUSED);
	}

	bool RecorderSL::record_impl(const unsigned long duration)
	{
		if (this->current == kNumInputSampleBuffers)
			return false;

		SLuint32 state = SL_RECORDSTATE_STOPPED;
		slGetRecordState(this->interface, &state);
		if (state == SL_RECORDSTATE_RECORDING)
			return true;

		slEnqueue(this->buffer_queue, this->buffer, kInputSamples * sizeof(short));
		slSetDurationLimit(this->interface, (duration ? duration : SL_TIME_UNKNOWN));
		return slSetRecordState(this->interface, SL_RECORDSTATE_RECORDING) == SL_RESULT_SUCCESS;
	}

	void RecorderSL::stop_impl()
	{
		if (this->current == kNumInputSampleBuffers)
			return;

		slSetRecordState(this->interface, SL_RECORDSTATE_STOPPED);
		slClear(this->buffer_queue);
		this->current = 0;
	}

	void RecorderSL::update_impl()
	{
		if (this->current == kNumInputSampleBuffers)
			return;

		const short *samples = this->get_buffer(this->current);
		for (size_t i = 0; i < kInputSamples; ++i)
		{
			const float power = fabsf(samples[i]);
			this->average += power;
			if (power > this->peak)
				this->peak = power;
		}
		this->average = decibels(this->average * (kNormalizeSample / kInputSamples));
		this->peak = decibels(this->peak * kNormalizeSample);
	}
}

#endif
