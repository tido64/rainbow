namespace Rainbow
{
	namespace ConFuoco
	{
		inline Mixer& Mixer::Instance()
		{
			static Mixer mixer;
			return mixer;
		}

		inline void Mixer::set_doppler_factor(const float f)
		{
			alDopplerFactor(f);
		}

		inline void Mixer::set_doppler_velocity(const float v)
		{
			alDopplerFactor(v);
		}

		inline void Mixer::set_speed_of_sound(const float s)
		{
			alSpeedOfSound(s);
		}

		inline void Mixer::set_gain(const float g)
		{
			alListenerf(AL_GAIN, (g < 0) ? 0.0f : g);
		}

		inline void Mixer::set_orientation(const float r)
		{
			float ori[] = { cosf(r + kPi_2), sinf(r + kPi_2), 0.0f, 0.0f, 0.0f, 1.0f };
			alListenerfv(AL_ORIENTATION, ori);
		}

		inline void Mixer::set_pitch(float pitch)
		{
			if (pitch < 0.5f)
				pitch = 0.5f;
			else if (pitch > 2.0f)
				pitch = 2.0f;
			alListenerf(AL_PITCH, pitch);
		}

		inline void Mixer::set_position(const float x, const float y, const float z)
		{
			alListener3f(AL_POSITION, x, y, z);
		}

		inline void Mixer::set_velocity(const float x, const float y, const float z)
		{
			alListener3f(AL_VELOCITY, x, y, z);
		}

		inline void Mixer::set_looping(const unsigned int source, const bool looping)
		{
			alSourcei(source, AL_LOOPING, looping ? AL_TRUE : AL_FALSE);
		}

		inline void Mixer::set_pitch(const unsigned int source, float pitch)
		{
			if (pitch < 0.5f)
				pitch = 0.5f;
			else if (pitch > 2.0f)
				pitch = 2.0f;
			alSourcef(source, AL_PITCH, pitch);
		}

		inline void Mixer::set_position(const unsigned int source, const float x, const float y, const float z)
		{
			alSource3f(source, AL_POSITION, x, y, z);
		}

		inline void Mixer::set_velocity(const unsigned int source, const float x, const float y, const float z)
		{
			alSource3f(source, AL_VELOCITY, x, y, z);
		}

	#ifdef RAINBOW_IOS

		inline void Mixer::set_bgm_pan(float p)
		{
			if (p > 1.0f)
				p = 1.0f;
			else if (p < -1.0f)
				p = -1.0f;
			this->player.pan = p;
		}

		inline void Mixer::set_bgm_volume(float v)
		{
			if (v > 1.0f)
				v = 1.0f;
			else if (v < 0.0f)
				v = 0.0f;
			this->player.volume = v;
		}

	#endif
	}
}
