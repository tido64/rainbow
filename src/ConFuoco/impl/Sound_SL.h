#ifndef CONFUOCO_SOUND_SL_H_
#define CONFUOCO_SOUND_SL_H_

#include <cstdlib>
#include <SLES/OpenSLES.h>

#include "ConFuoco/Wave.h"

namespace ConFuoco
{
	struct Player
	{
		SLObjectItf player;
		SLPlayItf play_itf;
		SLVolumeItf volume_itf;
		SLBufferQueueItf buffer_queue_itf;
	};

	class Sound : public Wave
	{
	public:
		Sound(const unsigned int i);
		virtual ~Sound() override;

		virtual bool load(const char *const file) override;

		virtual void set_gain(const float gain) override;
		virtual void set_loops(const int loops) override;

		virtual void pause() override;
		virtual void play(const float x = 0.0f, const float y = 0.0f, const float z = 0.0f) override;
		virtual void stop() override;

	private:
		bool paused;
		const size_t count;
		size_t current;
		size_t size;
		char *buffer;
		Player *players;

		void release();
	};
}

#endif
