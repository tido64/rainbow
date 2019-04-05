// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef CONFIG_H_
#define CONFIG_H_

namespace rainbow
{
    /// <summary>Load game configuration.</summary>
    /// <remarks>
    ///   Configuration is a simple INI file. File must be named
    ///   <c>config.ini</c> and reside in app root.
    ///
    ///   If no configuration file is present, or the file is somehow
    ///   unavailable, the following are the default values:
    ///
    ///   <code>
    ///   [core]
    ///   ResolutionWidth = 1280
    ///   ResolutionHeight = 720
    ///   MSAA = 0
    ///   AllowHiDPI = false
    ///   SuspendOnFocusLost = true
    ///   Accelerometer = false
    ///   </code>
    /// </remarks>
    class Config
    {
    public:
        Config();

        /// <summary>Returns the width of the screen.</summary>
        auto width() const { return width_; }

        /// <summary>Returns the height of the screen.</summary>
        auto height() const { return height_; }

        /// <summary>
        ///   Returns whether to create windows in HiDPI mode.
        /// </summary>
        bool hidpi() const { return hidpi_; }

        /// <summary>Returns whether the screen is in portrait mode.</summary>
        bool is_portrait() const { return width_ < height_; }

        /// <summary>
        ///   Returns number of samples for multisample anti-aliasing.
        /// </summary>
        auto msaa() const { return msaa_; }

        /// <summary>Returns whether we need to use the accelerometer.</summary>
        bool needs_accelerometer() const { return accelerometer_; }

        /// <summary>Returns whether to suspend when focus is lost.</summary>
        bool suspend() const { return suspend_; }

    private:
        int width_;
        int height_;
        unsigned int msaa_;
        bool hidpi_;
        bool suspend_;
        bool accelerometer_;
    };
}  // namespace rainbow

#endif
