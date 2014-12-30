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
	/// - <tt>allow_high_dpi = false|true</tt>
	///   Specifies whether to create windows in high DPI mode.
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
	/// - <tt>allow_high_dpi = false</tt>
	/// - <tt>resolution = { 0, 0 }</tt> (which also implies landscape mode)
	/// - <tt>suspend_on_focus_lost = true</tt>
	class Config
	{
	public:
		Config();

		/// Returns the width of the screen.
		int width() const { return width_; }

		/// Returns the height of the screen.
		int height() const { return height_; }

		/// Returns whether to create windows in high DPI mode.
		bool high_dpi() const { return high_dpi_; }

		/// Returns whether the screen is in portrait mode.
		bool is_portrait() const { return width_ < height_; }

		/// Returns whether we need to use the accelerometer.
		bool needs_accelerometer() const { return accelerometer_; }

		/// Returns whether to suspend when focus is lost.
		bool suspend() const { return suspend_; }

	private:
		bool accelerometer_;
		bool high_dpi_;
		bool suspend_;
		int width_;
		int height_;
	};
}

#endif
