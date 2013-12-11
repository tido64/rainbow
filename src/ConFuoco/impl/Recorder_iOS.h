#ifndef CONFUOCO_IMPL_RECORDER_IOS_H_
#define CONFUOCO_IMPL_RECORDER_IOS_H_

@class AVAudioRecorder;

namespace ConFuoco
{
	class RecorderiOS : public RecorderBase<RecorderiOS>
	{
		friend RecorderBase<RecorderiOS>;

	public:
		RecorderiOS();

	private:
		AVAudioRecorder *recorder;

		void pause_impl();
		bool record_impl(const unsigned long duration);
		void stop_impl();
		void update_impl();
	};

	typedef RecorderiOS Recorder;
}

#endif  // CONFUOCO_RECORDER_IOS_H_
