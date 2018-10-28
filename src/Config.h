// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef CONFIG_H_
#define CONFIG_H_

namespace rainbow
{
    namespace detail
    {
        class ConfigSAXParser;
    }

    /// <summary>Load game configuration.</summary>
    /// <remarks>
    ///   Configuration is a simple JSON file. File must be named
    ///   <c>config.json</c> and reside in app root. Please use the schema
    ///   <c>js/rainbow-config.schema.json</c> for IntelliSense:
    ///
    ///   <code>
    ///   {
    ///     "$schema": "./rainbow-config.schema.json"
    ///   }
    ///   </code>
    ///
    ///   If no configuration file is present, or the file is somehow
    ///   unavailable, the following are the default values:
    ///
    ///   <code>
    ///   {
    ///     "$schema": "./rainbow-config.schema.json",
    ///     "accelerometer": true,
    ///     "allowHighDPI": false,
    ///     "msaa": 0,
    ///     "resolution": {
    ///       "width": 0,
    ///       "height": 0
    ///     },
    ///     "suspendOnFocusLost": true
    ///   }
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
        ///   Returns whether to create windows in high DPI mode.
        /// </summary>
        bool high_dpi() const { return high_dpi_; }

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
        bool accelerometer_;
        bool high_dpi_;
        bool suspend_;
        int width_;
        int height_;
        unsigned int msaa_;

        friend detail::ConfigSAXParser;
    };
}  // namespace rainbow

#endif
