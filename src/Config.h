// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

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
	/// - <tt>suspend_on_focus_lost = false|true</tt>
	///   Specifies whether to suspend when focus is lost.
	///
	/// If no configuration file is present, or the file is somehow unavailable,
	/// the following are the default values:
	///
	/// - <tt>accelerometer = true</tt>
	/// - <tt>resolution = { 0, 0 }</tt> (which also implies landscape mode)
	/// - <tt>suspend_on_focus_lost = true</tt>
	class Config
	{
	public:
		Config();

		/// Returns the width of the screen.
		inline int width() const;

		/// Returns the height of the screen.
		inline int height() const;

		/// Returns whether the screen is in portrait mode.
		inline bool is_portrait() const;

		/// Returns whether we need to use the accelerometer.
		inline bool needs_accelerometer() const;

		/// Returns whether to suspend when focus is lost.
		inline bool suspend() const;

	private:
		bool accelerometer_;
		bool suspend_;
		int width_;
		int height_;
	};

	int Config::width() const
	{
		return width_;
	}

	int Config::height() const
	{
		return height_;
	}

	bool Config::is_portrait() const
	{
		return width_ < height_;
	}

	bool Config::needs_accelerometer() const
	{
		return accelerometer_;
	}

	bool Config::suspend() const
	{
		return suspend_;
	}
}

#endif
