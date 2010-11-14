/// A generic game element.

/// Copyright 2010 __MyCompanyName__. All rights reserved.
/// \author Tommy Nguyen

#ifndef ELEMENT_H_
#define ELEMENT_H_

class Element
{
public:
	Element() { }
	~Element() { }
	virtual void fire() = 0;
};

#endif
