#ifndef COMMON_NONCOPYABLE_H_
#define COMMON_NONCOPYABLE_H_

/// Prevents objects of a class from being copy-constructed or assigned to
/// each other.
///
/// Copyright 2012-13 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen
template<class T>
class NonCopyable
{
protected:
	NonCopyable() = default;
	~NonCopyable() = default;

private:
	NonCopyable(const NonCopyable &) = delete;
	NonCopyable& operator=(const NonCopyable &) = delete;
};

#endif
