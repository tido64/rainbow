/*
 *  HUD.h
 *  OnWire
 *
 *  Created by Tommy Nguyen on 7/9/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef HUD_H_
#define HUD_H_

#include "Label.h"
#include "Screen.h"

#define LABEL_FONT		"ArialBold.fnt"
#define LABEL_MARGIN	10
#define LABEL_SIZE		0.4f
#define PADDING			8
#define PROGRESS_FONT	"ArialBlack.fnt"
#define PROGRESS_SIZE	0.85f
#define TOTAL_MARGIN	14

class HUD
{
public:
	static const char distance_format[], time_format[];

	HUD();
	~HUD();

	Label *get_distance() { return this->distance; }
	Label *get_distance_label() { return this->distance_label; }
	Label *get_time() { return this->time; }
	Label *get_time_label() { return this->time_label; }

	void reset(const unsigned int target);
	void update_distance(const unsigned int d);
	void update_time(const unsigned int t);

private:
	char buffer[16];
	Label *distance, *distance_label, *time, *time_label;
};

#endif
