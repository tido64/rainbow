/// A generic game action.

/// Copyright 2010 __MyCompanyName__. All rights reserved.
/// \author Tommy Nguyen

class GameAction
{
public:
	GameAction() { }
	~GameAction() { }
	virtual void fire() = 0;
};
