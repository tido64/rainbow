#ifndef NONCOPYABLE_H_
#define NONCOPYABLE_H_

/// Prevent objects of a class from being copy-constructed or assigned to
/// each other.
///
/// Copyright 2012 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen
template<class T>
class NonCopyable
{
protected:
	NonCopyable() { }
	~NonCopyable() { }

private:
	NonCopyable(const NonCopyable &);
	NonCopyable& operator=(const NonCopyable &);
};

#endif
