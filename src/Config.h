#ifndef CONFIG_H_
#define CONFIG_H_

namespace Rainbow
{
	/// Load game configuration.
	///
	/// Rudimentary config-file parser. File must be named \c config and reside
	/// next to \c main.lua. Currently supports the following keys:
	///
	/// - <tt>accelerometer = false|true</tt>
	///   Specifies whether the accelerometer is used.
	/// - <tt>resolution = { width, height }</tt>
	///   Specifies the preferred screen resolution or window size. It also
	///   determines whether we are in landscape or portrait mode.
	///
	/// If no configuration file is present, or the file is somehow unavailable,
	/// the following are the default values:
	///
	/// - <tt>accelerometer = true</tt>
	/// - <tt>resolution = { 0, 0 }</tt> (which also implies landscape mode)
	///
	/// Copyright 2012-13 Bifrost Entertainment. All rights reserved.
	/// \author Tommy Nguyen
	class Config
	{
	public:
		Config();

		/// Returns the width of the screen.
		inline int get_width() const;

		/// Returns the height of the screen.
		inline int get_height() const;

		/// Returns whether the screen is in portrait mode.
		inline bool is_portrait() const;

		/// Returns whether we need to use the accelerometer.
		inline bool needs_accelerometer() const;

	private:
		bool accelerometer;
		int width;
		int height;
	};

	int Config::get_width() const
	{
		return this->width;
	}

	int Config::get_height() const
	{
		return this->height;
	}

	bool Config::is_portrait() const
	{
		return this->width < this->height;
	}

	bool Config::needs_accelerometer() const
	{
		return this->accelerometer;
	}
}

#endif
