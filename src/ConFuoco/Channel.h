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
		/// Whether this channel is paused.
		bool is_paused();

		/// Whether this channel is playing. Note that this function returns
		/// \c true as long as the channel is not stopped.
		bool is_playing();

		/// Set channel gain/volume.
		void set_gain(const float gain);

		/// Set channel on pause.
		void pause();

		/// Stop channel.
		void stop();

	private:
		unsigned int ch;  ///< Channel number.
		Mixer *mixer;     ///< Mixer object owning this channel.
		Sound *sound;     ///< The sound object bound to this channel.

		Channel();
	};
}

#endif
