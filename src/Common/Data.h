#ifndef DATA_H_
#define DATA_H_

#include <cstddef>

#include "Common/NonCopyable.h"
#include "Platform/Macros.h"

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
/// Copyright 2011-13 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen
class Data : private NonCopyable<Data>
{
public:
	enum Ownership
	{
		kDataOwner,
		kDataReference
	};

	enum Type
	{
		kDataTypeAsset,
		kDataTypeDocument
	};

	/// Construct an empty data object. No memory will be allocated.
	inline Data();

	/// Construct a data object with the contents of the file. The file's
	/// location will be determined by its type.
	Data(const char *const file, const Type type = kDataTypeAsset);

	/// Construct a wrapper around a buffer.
	inline Data(const void *buffer, const size_t size, const Ownership ownership);

	~Data();

	/// Return raw byte array.
	/// \return Pointer to array. Returns \c nullptr if buffer is empty.
	inline void* bytes() const;

	/// Save data to file.
	/// \return \c true on success, \c false otherwise.
	bool save(const char *const file) const;

	/// Return the size of this buffer.
	inline size_t size() const;

	inline operator bool() const;
	inline operator void*() const;
	inline operator char*() const;
	inline operator unsigned char*() const;

#ifdef RAINBOW_IOS
	inline operator NSData*() const;
	inline operator NSMutableData*() const;
#endif

private:
	Ownership ownership;  ///< Decides whether to free the buffer on destruction.
	size_t allocated;     ///< Allocated memory size.
	size_t sz;            ///< Size of used buffer, not necessarily equal to allocated.
	void *data;           ///< Actual buffer, implemented as a C-array.

	/// Resize allocated memory segment. If the requested allocation size is
	/// smaller than current allocated size, nothing will happen.
	void allocate(const size_t size);
};

Data::Data() : ownership(kDataOwner), allocated(0), sz(0), data(nullptr) { }

Data::Data(const void *buffer, const size_t size, const Ownership ownership) :
	ownership(ownership), allocated(size), sz(size),
	data(const_cast<void*>(buffer)) { }

void* Data::bytes() const
{
	return this->data;
}

size_t Data::size() const
{
	return this->sz;
}

Data::operator bool() const
{
	return this->data;
}

Data::operator void*() const
{
	return this->data;
}

Data::operator char*() const
{
	return static_cast<char*>(this->data);
}

Data::operator unsigned char*() const
{
	return static_cast<unsigned char*>(this->data);
}

#ifdef RAINBOW_IOS

Data::operator NSData*() const
{
	return [NSData dataWithBytesNoCopy:this->data length:this->sz freeWhenDone:NO];
}

Data::operator NSMutableData*() const
{
	return [NSMutableData dataWithBytesNoCopy:this->data length:this->sz freeWhenDone:NO];
}

#endif  // RAINBOW_IOS
#endif  // DATA_H_
