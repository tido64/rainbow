// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

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
