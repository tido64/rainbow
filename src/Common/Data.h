#ifndef DATA_H_
#define DATA_H_

#include "Platform.h"

#if defined(RAINBOW_IOS)
#else
#	include <cassert>
#	include <cstdio>
#	include <cstring>
#endif

/// Wrapper for byte buffers.
///
/// Data objects are used to read from disk into memory or to save files to
/// disk.
///
/// Data is "toll-free bridged" with its platform-specific counterparts. On
/// iOS, it is safe to perform a static cast of this type to an NSData or
/// NSMutableData (which in turn can be cast to a CFData or CFMutableData).
/// On any other platforms, it can be casted to a void pointer or an unsigned
/// char pointer.
///
/// Copyright 2011 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen
class Data
{
public:
#if defined(RAINBOW_IOS)

	Data() : data(nil) { }
	~Data() { [data release]; }

#else

	Data() : sz(0), data(0) { }
	~Data() { delete[] this->data; }

#endif

	/// Return raw byte array.
	/// \return Pointer to array. If 0, then buffer is empty.
	unsigned char* bytes() const;

	/// Load data from file.
	/// \return True on success, false otherwise.
	bool load(const char *const file);

	/// Save data to file.
	/// \return True on success, false otherwise.
	bool save(const char *const file) const;

	/// Return the size of this buffer.
	unsigned int size() const;

	operator void*() const;
	operator unsigned char*() const;

#ifdef RAINBOW_IOS

	operator NSData*() const;
	operator NSMutableData*() const;

private:
	NSMutableData *data;

#else

private:
	unsigned int sz;
	unsigned char *data;

#endif
};

#ifdef RAINBOW_IOS

inline unsigned char* Data::bytes() const
{
	return static_cast<unsigned char *>(data.mutableBytes);
}

inline unsigned int Data::size() const
{
	return data.length;
}

inline Data::operator void*() const
{
	return data.mutableBytes;
}

inline Data::operator unsigned char*() const
{
	return static_cast<unsigned char *>(data.mutableBytes);
}

inline Data::operator NSData*() const
{
	return static_cast<NSData *>(this->data);
}

inline Data::operator NSMutableData*() const
{
	return this->data;
}

#else

inline unsigned char* Data::bytes() const
{
	return this->data;
}

inline unsigned int Data::size() const
{
	return this->sz;
}

inline Data::operator void*() const
{
	return this->data;
}

inline Data::operator unsigned char*() const
{
	return this->data;
}

#endif

#endif
