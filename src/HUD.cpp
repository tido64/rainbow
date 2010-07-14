/*
 *  HUD.cpp
 *  OnWire
 *
 *  Created by Tommy Nguyen on 7/9/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "HUD.h"

const char HUD::distance_format[] = "%um";
const char HUD::time_format[] = "%u:%.2u";

HUD::HUD()
{
	this->distance = new Label("9000m", PROGRESS_FONT);
	this->distance_label = new Label("Distance", LABEL_FONT);
	this->time = new Label("0:00", PROGRESS_FONT);
	this->time_label = new Label("Time", LABEL_FONT);

	// Set vertical alignment to bottom
	this->distance->set_valign(0.0f);
	this->distance_label->set_valign(0.0f);
	this->time->set_valign(0.0f);
	this->time_label->set_valign(0.0f);

	// Set horizontal alignment
	this->distance->align_left();
	this->distance_label->align_left();
	this->time->align_right();
	this->time_label->align_right();

	// Set font size
	this->distance->scale(PROGRESS_SIZE);
	this->distance_label->scale(LABEL_SIZE);
	this->time->scale(PROGRESS_SIZE);
	this->time_label->scale(LABEL_SIZE);

	// Calculate label position on screen
	Screen *s = Screen::Instance();
	const unsigned int
		w = s->get_width(),
		top_pos = s->get_height() - this->distance->get_height() + PADDING,
		top_label_pos = s->get_height() - this->distance_label->get_height() + PADDING;

	// Set position of labels
	this->distance->set_position((this->distance_label->get_width() * LABEL_SIZE) + TOTAL_MARGIN, top_pos);
	this->distance_label->set_position(LABEL_MARGIN, top_label_pos);
	this->time->set_position(w - (this->time_label->get_width() * LABEL_SIZE) - TOTAL_MARGIN, top_pos);
	this->time_label->set_position(w - LABEL_MARGIN, top_label_pos);
}

HUD::~HUD()
{
	delete this->distance;
	delete this->distance_label;
	delete this->time;
	delete this->time_label;
}

void HUD::reset(const unsigned int target)
{
	this->time->set_text("0:00");
	this->update_distance(target);
}

void HUD::update_distance(const unsigned int d)
{
	sprintf(this->buffer, this->distance_format, d);
	this->distance->set_text(this->buffer);
}

void HUD::update_time(const unsigned int time)
{
	sprintf(this->buffer, this->time_format, time / 60, time % 60);
	this->time->set_text(this->buffer);
}
