// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef CONFUOCO_CHANNEL_H_
#define CONFUOCO_CHANNEL_H_

namespace ConFuoco
{
	class Mixer;
	class Sound;

	class Channel
	{
		friend class Mixer;

	public:
		/// Returns whether this channel is paused.
		bool is_paused();

		/// Returns whether this channel is playing. Note that this function
		/// returns \c true as long as the channel is not stopped.
		bool is_playing();

		/// Sets channel gain/volume.
		void set_gain(const float gain);

		/// Sets channel on pause.
		void pause();

		/// Stops playback.
		void stop();

	private:
		unsigned int ch;  ///< Channel number.
		Mixer *mixer;     ///< Mixer object owning this channel.
		Sound *sound;     ///< The sound object bound to this channel.

		Channel();
	};
}

#endif
