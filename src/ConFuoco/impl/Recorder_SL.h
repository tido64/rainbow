// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef CONFUOCO_IMPL_RECORDER_SL_H_
#define CONFUOCO_IMPL_RECORDER_SL_H_

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

namespace ConFuoco
{
    class RecorderSL : public RecorderBase<RecorderSL>
    {
        friend RecorderBase<RecorderSL>;

    public:
        RecorderSL();
        ~RecorderSL();

        void swap();

    private:
        static const size_t kInputSamples = 512;
        static const size_t kNumInputSampleBuffers = 2;

        unsigned int current;

        SLObjectItf object;
        SLRecordItf interface;
        SLAndroidSimpleBufferQueueItf buffer_queue;

        short buffer[kInputSamples * kNumInputSampleBuffers];

        short* get_buffer(const unsigned int i);
        unsigned int next() const;

        void pause_impl();
        bool record_impl(const unsigned long duration);
        void stop_impl();
        void update_impl();
    };

    using Recorder = RecorderSL;
}

#endif  // CONFUOCO_RECORDER_SL_H_
