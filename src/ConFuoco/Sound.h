// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

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

		virtual ~Sound() = default;

	protected:
		Sound(const Type t) : type(t) { }
	};
}

#endif
