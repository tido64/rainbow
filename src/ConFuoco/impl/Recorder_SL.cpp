// Copyright 2013 Bifrost Entertainment. All rights reserved.

#include "ConFuoco/Recorder.h"
#ifdef RAINBOW_OS_ANDROID

#include <climits>
#include <cmath>

#include "Common/Algorithm.h"
#include "ConFuoco/impl/Mixer_SL.h"
#include "ConFuoco/impl/Recorder_SL.h"

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
		const size_t kInputSamples = 512;
		const size_t kNumInputSampleBuffers = 2;

		short* active_buffer(const SLRecorder *recorder)
		{
			return recorder->buffer + kInputSamples * recorder->active_buffer;
		}

		float decibels(const float amplitude)
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

	SLRecorder::SLRecorder() :
		active_buffer(0), object(nullptr), interface(nullptr), buffer(nullptr), samples(nullptr)
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

		SLEngineItf engine = static_cast<ConFuoco::MixerSL*>(ConFuoco::Mixer::Instance)->interface();
		if (slCreateAudioRecorder(engine, &(this->object), &source, &sink, 1, iids, req) != SL_RESULT_SUCCESS)
		{
			R_ERROR(CF_TAG "Failed to create audio recorder\n");
			return;
		}
		if (slRealize(this->object, SL_BOOLEAN_FALSE) != SL_RESULT_SUCCESS)
		{
			R_ERROR(CF_TAG "Failed to realize audio recorder\n");
			return;
		}
		if (slGetInterface(this->object, SL_IID_RECORD, &this->interface) != SL_RESULT_SUCCESS)
		{
			R_ERROR(CF_TAG "Failed to get audio recorder interface\n");
			return;
		}

		const size_t sz = kInputSamples * kNumInputSampleBuffers;
		this->buffer = new short[sz];
		memset(this->buffer, 0, sz);
	}

	SLRecorder::~SLRecorder()
	{
		if (this->object)
			slDestroy(this->object);
		delete this->buffer;
	}

	void SLRecorder::set_callback(slAndroidSimpleBufferQueueCallback callback, void *context)
	{
		if (slGetInterface(this->object, SL_IID_ANDROIDSIMPLEBUFFERQUEUE, &this->buffer_queue) != SL_RESULT_SUCCESS)
		{
			R_ERROR(CF_TAG "Failed to get buffer queue interface\n");
			return;
		}
		if (slRegisterCallback(buffer_queue, callback, context) != SL_RESULT_SUCCESS)
		{
			R_ERROR(CF_TAG "Failed to set audio recorder callback\n");
			return;
		}
	}

	Recorder::Recorder() :
		average(0.0f), low_pass(0.0f), peak(0.0f), recorder(nullptr)
	{
		this->recorder = new SLRecorder();
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

#endif
