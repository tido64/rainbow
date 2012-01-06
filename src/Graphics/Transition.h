#ifndef TRANSITION_H_
#define TRANSITION_H_

/// Transition of a property from one value to another.
///
/// Copyright 2011-12 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen
class Transition
{
public:
	static Transition* create(float &property, const float final, const unsigned int duration, const int transition);

	virtual ~Transition() { }
	virtual bool update() = 0;
};

namespace Rainbow
{
	template<class T>
	class _Transition : public Transition
	{
	public:
		inline _Transition(float &property, const float final, const unsigned int duration = 1000);
		virtual bool update();

	private:
		T transition;
		bool stopped;
		const float final;
		const float initial;
		float &property;
		const unsigned int duration;
		unsigned int elapsed;
	};

	template<class T>
	_Transition<T>::_Transition(float &p, const float f, const unsigned int d) :
		stopped(false), final(f), initial(p), property(p), duration(d), elapsed(0) { }
}

#endif
