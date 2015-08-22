// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef CONFIG_H_
#define CONFIG_H_

namespace rainbow
{
	/// <summary>Load game configuration.</summary>
	/// <remarks>
	///   Rudimentary config-file parser. File must be named <c>config</c> and
	///   reside next to <c>main.lua</c>. Currently supports the following keys:
	///
	///   <list type="bullet">
	///     <item>
	///       <c>accelerometer = false|true</c><br/>
	///       Specifies whether the accelerometer is used.
	///     </item>
	///     <item>
	///       <c>allow_high_dpi = false|true</c><br/>
	///       Specifies whether to create windows in high DPI mode.
	///     </item>
	///     <item>
	///       <c>msaa = 0|2|4|8</c><br/>
	///       Sets number of samples for multisample anti-aliasing.
	///     </item>
	///     <item>
	///       <c>resolution = {width, height}</c><br/>
	///       Specifies the preferred screen resolution or window size. It also
	///       determines whether we are in landscape or portrait mode.
	///     </item>
	///     <item>
	///       <c>suspend_on_focus_lost = false|true</c><br/>
	///       Specifies whether to suspend when focus is lost.
	///     </item>
	///   </list>
	///
	///   If no configuration file is present, or the file is somehow
	///   unavailable, the following are the default values:
	///
	///   <list type="bullet">
	///     <item><c>accelerometer = true</c></item>
	///     <item><c>allow_high_dpi = false</c></item>
	///     <item><c>msaa = 0</c></item>
	///     <item><c>resolution = {0, 0}</c> (which also implies landscape mode)</item>
	///     <item><c>suspend_on_focus_lost = true</c></item>
	///   </list>
	/// </remarks>
	class Config
	{
	public:
		Config();

		/// <summary>Returns the width of the screen.</summary>
		int width() const { return width_; }

		/// <summary>Returns the height of the screen.</summary>
		int height() const { return height_; }

		/// <summary>
		///   Returns whether to create windows in high DPI mode.
		/// </summary>
		bool high_dpi() const { return high_dpi_; }

		/// <summary>Returns whether the screen is in portrait mode.</summary>
		bool is_portrait() const { return width_ < height_; }

		/// <summary>
		///   Returns number of samples for multisample anti-aliasing.
		/// </summary>
		unsigned int msaa() const { return msaa_; }

		/// <summary>Returns whether we need to use the accelerometer.</summary>
		bool needs_accelerometer() const { return accelerometer_; }

		/// <summary>Returns whether to suspend when focus is lost.</summary>
		bool suspend() const { return suspend_; }

	private:
		bool accelerometer_;
		bool high_dpi_;
		bool suspend_;
		int width_;
		int height_;
		unsigned int msaa_;
	};
}

#endif
