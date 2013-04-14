// Copyright 2013 Bifrost Entertainment. All rights reserved.

#include "ConFuoco/Recorder.h"
#ifdef RAINBOW_SDL

namespace ConFuoco
{
	Recorder::Recorder() :
		average(0.0f), low_pass(0.0f), peak(0.0f), recorder(nullptr) { }

	Recorder::~Recorder() { }
	void Recorder::pause() { }
	bool Recorder::record(const unsigned long) { return false; }
	void Recorder::stop() { }
	void Recorder::update() { }
}

#endif
