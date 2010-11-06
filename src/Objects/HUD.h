/// Head-up display for game progression.

/// Displays on-screen player progress and how much time he has used.
///
/// Copyright 2010 __MyCompanyName__. All rights reserved.
/// \author Tommy Nguyen

#ifndef HUD_H_
#define HUD_H_

#include "../Framework/Label.h"
#include "../Hardware/Screen.h"

class HUD
{
public:
	HUD();
	~HUD();

	/// Returns the distance label. Used for adding it to the director.
	/// \return Distance label
	Label *get_distance() { return this->distance; }

	/// Returns the label for distance. Used for adding it to the director.
	/// \return Label for distance
	Label *get_distance_label() { return this->distance_label; }

	/// Returns the time label. Used for adding it to the director.
	/// \return Time label
	Label *get_time() { return this->time; }

	/// Returns the label for time. Used for adding it to the director.
	/// \return Label for time
	Label *get_time_label() { return this->time_label; }

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
