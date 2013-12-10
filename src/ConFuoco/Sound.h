#ifndef CONFUOCO_SOUND_H_
#define CONFUOCO_SOUND_H_

namespace ConFuoco
{
	class Sound
	{
	public:
		enum class Type
		{
			Static,
			Stream
		};

		const Type type;

		virtual ~Sound() { }

	protected:
		Sound(const Type t) : type(t) { }
	};
}

#endif
