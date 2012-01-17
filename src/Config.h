#ifndef CONFIG_H_
#define CONFIG_H_

/// Load game configuration.
///
/// Rudimentary config-file parser. File must be named \c config and reside next
/// to \c main.lua. Currently supports the following keys:
///
/// - <tt>accelerometer: 0|1</tt>
///   Specifies whether the accelerometer is used.
/// - <tt>resolution: <width>x<height></tt>
///   Specifies the intended screen resolution or window size. It also
///   determines whether we are in landscape or portrait mode.
///
/// If no configuration file is present, or the file is somehow unavailable, the
/// following are the default values:
///
/// - <tt>accelerometer: 1</tt>
/// - <tt>resolution: 0x0</tt> (which also implies landscape mode)
///
/// Copyright 2012 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen
class Config
{
public:
	Config();

	/// Return the width of the screen.
	inline int get_width() const;

	/// Return the height of the screen.
	inline int get_height() const;

	/// Whether the screen is in portrait mode.
	inline bool is_portrait() const;

	/// Whether we need to use the accelerometer.
	inline bool needs_accelerometer() const;

private:
	bool accelerometer;
	int width;
	int height;

	bool keycmp(const char *line, const char *key);
	bool parse_accelerometer(const char *line, const unsigned int length);
	bool parse_resolution(const char *line, const unsigned int length);
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

#endif
