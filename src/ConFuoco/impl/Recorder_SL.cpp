// Copyright 2013 Bifrost Entertainment. All rights reserved.

#include "ConFuoco/Recorder.h"
#ifdef RAINBOW_ANDROID

#include <climits>
#include <cmath>

#include "Common/Algorithm.h"
#include "ConFuoco/Mixer.h"
#include "ConFuoco/impl/Recorder_SL.h"

#define slClear(self)                   (*self)->Clear(self)
#define slEnqueue(self, buffer, size)   (*self)->Enqueue(self, buffer, size)
#define slGetRecordState(self, state)   (*self)->GetRecordState(self, state)
#define slSetDurationLimit(self, msec)  (*self)->SetDurationLimit(self, msec)
#define slSetRecordState(self, state)   (*self)->SetRecordState(self, state)

namespace ConFuoco
{
	namespace
	{
		const float kNormalizeSample = 1.0f / SHRT_MAX;

		inline short* active_buffer(const SLRecorder *recorder)
		{
			return recorder->buffer + kInputSamples * recorder->active_buffer;
		}

		inline float decibels(const float amplitude)
		{
			return 20 * log10f(amplitude);
		}

		void AndroidSimpleBufferQueueCallback(SLAndroidSimpleBufferQueueItf, void *pContext)
		{
			SLRecorder *recorder = static_cast<SLRecorder*>(pContext);
			recorder->samples = active_buffer(recorder);
			recorder->active_buffer = (recorder->active_buffer + 1) % kNumInputSampleBuffers;
			slEnqueue(recorder->buffer_queue, active_buffer(recorder), kInputSamples * sizeof(short));
		}
	}

	Recorder::Recorder() :
		average(0.0f), low_pass(0.0f), peak(0.0f), recorder(nullptr)
	{
		this->recorder = static_cast<SLRecorder*>(Mixer::Instance->create_recorder());
		if (!this->recorder)
		{
			delete static_cast<SLRecorder*>(this->recorder);
			this->recorder = nullptr;
			return;
		}
		this->recorder->set_callback(AndroidSimpleBufferQueueCallback, this->recorder);
	}

	Recorder::~Recorder()
	{
		this->stop();
		delete this->recorder;
	}

	void Recorder::pause()
	{
		slSetRecordState(this->recorder->interface, SL_RECORDSTATE_PAUSED);
	}

	bool Recorder::record(const unsigned long duration)
	{
		SLuint32 state = SL_RECORDSTATE_STOPPED;
		slGetRecordState(this->recorder->interface, &state);
		if (state == SL_RECORDSTATE_RECORDING)
			return true;

		slEnqueue(this->recorder->buffer_queue, this->recorder->buffer, kInputSamples * sizeof(short));
		slSetDurationLimit(this->recorder->interface, duration ? duration : SL_TIME_UNKNOWN);
		return slSetRecordState(this->recorder->interface, SL_RECORDSTATE_RECORDING) == SL_RESULT_SUCCESS;
	}

	void Recorder::stop()
	{
		slSetRecordState(this->recorder->interface, SL_RECORDSTATE_STOPPED);
		slClear(this->recorder->buffer_queue);
		this->recorder->active_buffer = 0;
	}

	void Recorder::update()
	{
		if (!this->recorder->samples)
			return;

		for (size_t i = 0; i < kInputSamples; ++i)
		{
			const float power = fabsf(this->recorder->samples[i]);
			this->average += power;
			if (power > this->peak)
				this->peak = power;
		}
		this->average = decibels(this->average * (kNormalizeSample / kInputSamples));
		this->peak = decibels(this->peak * kNormalizeSample);
		this->recorder->samples = nullptr;

		this->low_pass = Rainbow::low_pass(this->peak, this->low_pass);
	}
}

#undef slSetRecordState
#undef slSetDurationLimit
#undef slGetRecordState
#undef slEnqueue
#undef slClear
#endif
