/// Head-up display for game progression.

/// Displays on-screen player progress and how much time he has used.
///
/// Copyright 2010 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#ifndef HUD_H_
#define HUD_H_

#include <Rainbow/Hardware/Screen.h>
#include <Rainbow/FontAtlas.h>

class HUD
{
public:
	HUD();

	/// Draws head-up display.
	void draw();

	/// Resets all values.
	/// \param target The new target distance
	void reset(const unsigned int target);

	/// Sets the distance.
	/// \param d Distance left
	void set_distance(const unsigned int d);

	/// Sets the duration.
	/// \param t Time spent
	void set_time(const unsigned int t);

	void update();

protected:
	static const char
		distance_format[],  ///< Provides formatting for distance
		time_format[];      ///< Provides formatting for time

private:
	float distance_label;   ///< Position of distance label
	float time_label;       ///< Position of time label
	float top;              ///< y-component of label positions
	char distance_str[8];   ///< Distance label
	char time_str[8];       ///< Time label
	FontAtlas arial_black;  ///< Arial Black
	FontAtlas arial_bold;   ///< Arial Bold
};

#endif
