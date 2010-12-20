/// Wind game element affects the behaviour of the line.

/// Copyright 2010 __MyCompanyName__. All rights reserved.
/// \author Tommy Nguyen

#ifndef WIND_H_
#define WIND_H_

#include <Rainbow/Element.h>
#include "../Objects/Line.h"

class Wind : public Element
{
public:
	Wind(const Line *);
	void activate();
	bool fire();

private:
	float force;
	const Line *line;
};

#endif
