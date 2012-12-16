#ifndef CONFUOCO_SOUND_H_
#define CONFUOCO_SOUND_H_

namespace ConFuoco
{
	class Sound
	{
	public:
		const int type;

		virtual ~Sound() { }

	protected:
		Mixer *mixer;

		Sound(const int t, Mixer *m) : type(t), mixer(m) { }
	};
}

#endif
