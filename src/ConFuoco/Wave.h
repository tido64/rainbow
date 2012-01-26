#ifndef CONFUOCO_WAVE_H_
#define CONFUOCO_WAVE_H_

namespace ConFuoco
{
	/// Base wave class for static and streaming audio.
	///
	/// Waves can either be loaded entirely into buffer or streamed from disk
	/// in order to save memory. The latter method is particularly useful for
	/// longer audio files such as music or speech.
	///
	/// Copyright 2012 Bifrost Entertainment. All rights reserved.
	/// \author Tommy Nguyen
	class Wave
	{
	public:
		virtual ~Wave() { }

		/// Load audio file and prepare it for playing.
		/// \param file  Path to audio file.
		/// \return \c true on success, \c false otherwise.
		virtual bool load(const char *const file) = 0;

		/// Set source gain.
		/// \param gain  Scalar amplitude multiplier. The default 1.0 means
		///              that the sound is unattenuated. The value zero equals
		///              silence (no contribution to the output mix).
		virtual void set_gain(const float gain) = 0;

		/// Set the number of times a sound will return to the beginning, upon
		/// reaching the end, to repeat playback.
		/// \param loops  A value of 0 means to play the sound once. Set a
		///               positive integer value to specify the number of times
		///               to return to the start and play again. Set any
		///               negative integer value to loop the sound indefinitely
		///               until you call the stop method.
		virtual void set_loops(const int loops) = 0;

		/// Pause playback; sound remains ready to resume playback from where
		/// it left off.
		virtual void pause() const = 0;

		/// Play a sound asynchronously.
		/// \param x,y,z  Position of the sound.
		virtual void play(const float x = 0.0f, const float y = 0.0f, const float z = 0.0f) const = 0;

		/// Stop playback and undo the setup needed for playback.
		virtual void stop() const = 0;
	};
}

#endif
