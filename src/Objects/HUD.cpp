//
//  HUD.cpp
//  OnWire
//
//  Created by Tommy Nguyen on 7/9/10.
//  Copyright 2010 Bifrost Games. All rights reserved.
//

#include "HUD.h"

const char HUD::distance_format[]  = "%um";
const char HUD::time_format[]      = "%02u:%.2u";

HUD::HUD() :
	arial_black("ArialBlack.ttf", 20.0f),
	arial_bold("ArialBold.ttf", 10.0f)
{ }

void HUD::draw()
{
	this->arial_bold.print("Distance", this->distance_label, this->top);
	this->arial_bold.print("Time", this->time_label, this->top);
	this->arial_black.print(this->distance_str, this->distance_label + 58, this->top - 3);
	this->arial_black.print(this->time_str, this->time_label - 85, this->top - 3);
}

void HUD::reset(const unsigned int t)
{
	this->set_distance(t);
	this->set_time(0);
}

void HUD::set_distance(const unsigned int d)
{
	sprintf(this->distance_str, HUD::distance_format, d);
}

void HUD::set_time(const unsigned int t)
{
	sprintf(this->time_str, HUD::time_format, t / 60, t % 60);
}

void HUD::update()
{
	this->top = Screen::Instance().height() * 0.94375f;
	this->distance_label = Screen::Instance().width() * 0.0375f;
	this->time_label = Screen::Instance().width() * 0.8625f;
}
