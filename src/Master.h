/*
 *  Master.h
 *  OnWire
 *
 *  Created by Tommy Nguyen on 7/14/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef MASTER_H_
#define MASTER_H_

#include "HUD.h"

class Master
{
public:
	static Master *Instance();
	void elapse_time();
	float get_progress();
	unsigned int get_time();
	unsigned int get_traveled();
	void reset(const unsigned int target);
	void travel(const unsigned int distance);
	void use(HUD *);

private:
	bool finished;
	unsigned int target, time, traveled;
	HUD* hud;

	Master() : target(0), time(target), traveled(target), hud(0) { }
};

#endif
