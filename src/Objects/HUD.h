/// Head-up display for game progression.

/// Displays on-screen player progress and how much time he has used.
///
/// Copyright 2010 __MyCompanyName__. All rights reserved.
/// \author Tommy Nguyen

#ifndef HUD_H_
#define HUD_H_
#define HUD_COUNT 4

#include "../Framework/Label.h"
#include "../Hardware/Screen.h"

class HUD
{
public:
	HUD();
	~HUD();

	inline unsigned int get_element_count() { return HUD_COUNT; }

	RealLabel **get_elements();

	/// Resets the values in the HUD.
	/// \param target The new target distance
	void reset(const unsigned int target);

	/// Updates the distance.
	/// \param d Distance left
	void update_distance(const unsigned int d);

	/// Updates the duration.
	/// \param t Time spent
	void update_time(const unsigned int t);

private:
	static const unsigned int
		label_margin,		///< Margin between left edge of the screen and label
		padding,			///< Padding between label and value
		total_margin;		///< Total margin preceding the value
	static const float
		label_size,			///< Size of the describing label
		progress_size;		///< Size of the progress label
	static const char
		distance_format[],	///< Provides formatting for distance
		init_time[],		///< The initial time value
		label_font[],		///< Font name of the labels in the display
		progress_font[],	///< Font name of the numbers in the display
		time_format[];		///< Provides formatting for time
	char buffer[16];		///< Buffer for creating strings
	Label
		*distance,			///< Label for distance value
		*distance_label,	///< Label for distance label
		*time,				///< Label for time value
		*time_label;		///< Label for time label
};

#endif
