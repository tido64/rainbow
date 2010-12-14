/// Head-up display for game progression.

/// Displays on-screen player progress and how much time he has used.
///
/// Copyright 2010 __MyCompanyName__. All rights reserved.
/// \author Tommy Nguyen

#ifndef HUD_H_
#define HUD_H_

#include "../Framework/Font.h"
#include "../Framework/Label.h"
#include "../Hardware/Screen.h"

class HUD
{
public:
	HUD();
	~HUD();

	/// Resets all values.
	/// \param target The new target distance
	void reset(const unsigned int target);

	/// Sets the distance.
	/// \param d Distance left
	void set_distance(const unsigned int d);

	/// Sets the duration.
	/// \param t Time spent
	void set_time(const unsigned int t);

private:
	static const unsigned int
		label_margin,       ///< Margin between left edge of the screen and label
		padding,            ///< Padding between label and value
		total_margin;       ///< Total margin preceding the value
	static const float
		label_size,         ///< Size of the describing label
		progress_size;      ///< Size of the progress label
	static const char
		distance_format[],  ///< Provides formatting for distance
		init_time[],        ///< The initial time value
		label_font[],       ///< Font name of the labels in the display
		progress_font[],    ///< Font name of the numbers in the display
		time_format[];      ///< Provides formatting for time

	char buffer[16];        ///< Buffer for creating strings
	Label *distance;        ///< Label for distance value
	Label *distance_label;  ///< Label for distance text
	Label *time;            ///< Label for time value
	Label *time_label;      ///< Label for time text

	friend class OnWireGame;
};

#endif
