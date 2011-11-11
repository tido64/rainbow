#ifndef AUDIO_MIXER_H_
#define AUDIO_MIXER_H_

#include <cmath>

#include "Common/Constants.h"
#include "ConFuoco/OpenAL.h"

#ifdef RAINBOW_IOS
#	include <AVFoundation/AVFoundation.h>
#	include <AudioToolbox/AudioServices.h>
#	include "Wave.h"
#else
#	include "Stream.h"
#endif

#define AUDIO_BUFFERED 1       ///< Flagged when AL buffers have been generated
#define AUDIO_SOURCED  1 << 1  ///< Flagged when AL sources have been generated
#define AUDIO_BUFFERS  16      ///< Number of different sounds
#define AUDIO_SOURCES  16      ///< Number of simultaneous sounds (excl. bgm)

namespace Rainbow
{
	namespace ConFuoco
	{
		/// Mixes audio streams.
		///
		/// \see http://www.devmaster.net/articles/openal-tutorials/lesson8.php
		///
		/// Copyright 2011 Bifrost Games. All rights reserved.
		/// \author Tommy Nguyen
		class Mixer
		{
		public:
			static Mixer& Instance();

			~Mixer();

			/// Clear all buffers.
			void clear();

			/// Perform source array clean up. On some platforms, also update
			/// stream buffers.
			void update();


			/* Sound properties */

			/// Set Doppler factor.
			void set_doppler_factor(const float);

			/// Set Doppler velocity.
			void set_doppler_velocity(const float);

			/// Set speed of sound.
			void set_speed_of_sound(const float);


			/* Listener controls */

			/// Set post-mixed audio gain.
			void set_gain(const float g);

			/// Set orientation of the listener.
			/// \param r  Orientation in radians
			void set_orientation(const float r);

			/// Set post-mixed audio pitch.
			/// \param pitch  Sound pitch [0.5, 2.0]
			void set_pitch(float pitch = 1.0f);

			/// Set position of the listener.
			void set_position(const float x = 0, const float y = 0, const float z = 0);

			/// Set velocity of the listener.
			void set_velocity(const float x = 0, const float y = 0, const float z = 0);


			/* Source controls */

			/// Add sound to mixer.
			/// \return Audio buffer id
			unsigned int add(const char *const);

			/// Play sound wave at given position.
			/// \return Source id
			unsigned int play(const unsigned int id, const float x = 0, const float y = 0, const float z = 0);

			/// Set source looping.
			void set_looping(const unsigned int source, const bool looping);

			/// Set sound pitch at source.
			/// \param source  Source id
			/// \param pitch   Sound pitch [0.5, 2.0]
			void set_pitch(const unsigned int source, float pitch);

			/// Set position of the sound source.
			void set_position(const unsigned int source, const float x = 0, const float y = 0, const float z = 0);

			/// Set velocity of the sound source.
			void set_velocity(const unsigned int source, const float x = 0, const float y = 0, const float z = 0);


			/* Background music controls */

			/// Set stream as background music.
			void set_bgm(const char *const media);

			/// Play background music.
			void play_bgm();

			/// Pause background music.
			void pause_bgm();

			/// Stop playing background music and optionally clear buffer.
			void stop_bgm(bool clear = false);

			/// Set background music panning.
			/// \param p  -1.0 is left, 0.0 is center, 1.0 is right
			void set_bgm_pan(float p = 0.0f);

			/// Set background music volume.
			/// \param v  Volume [0.0, 1.0]
			void set_bgm_volume(float v);

		#ifdef RAINBOW_IOS

		protected:
			AVAudioPlayer *player;

			static void InterruptionListener(void *, UInt32);
			static void RouteChangeListener(void *, AudioSessionPropertyID, UInt32, const void *);

			void init_audio_session();

		#else

		protected:
			Stream stream;  ///< Dedicated background music stream

		#endif

		private:
			bool interrupted;                     ///< Audio mixer got interrupted by external events
			unsigned char cleanup;                ///< Used internally for cleaning up AL lib
			unsigned int busy;                    ///< Whether the audio device is busy (iPod, etc.)
			unsigned int buffer_count;            ///< Number of buffered sound streams in use
			unsigned int source_count;            ///< Number of sound sources in use

			unsigned int buffers[AUDIO_BUFFERS];  ///< Sound buffers (number of sound streams)
			unsigned int sources[AUDIO_SOURCES];  ///< Audio sources (determines how many sounds can be played simultaneously)
			ALCcontext *context;                  ///< OpenAL context

			Wave waves[AUDIO_BUFFERS];            ///< Array of sound effects loaded into memory

			/// Initialise and set up audio session.
			Mixer();

			/// Intentionally left undefined.
			Mixer(const Mixer &);

			/// Initialise OpenAL device and buffers.
			void init_AL();

			/// Intentionally left undefined.
			Mixer& operator=(const Mixer &);
		};
	}
}

#include "ConFuoco/impl/Mixer-inl.h"

#endif
