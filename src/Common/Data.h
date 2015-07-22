// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_DATA_H_
#define COMMON_DATA_H_

#include "Common/Constraints.h"
#include "Common/NonCopyable.h"
#include "Platform/Macros.h"

class File;

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
class Data : private NonCopyable<Data>
{
public:
	enum class Ownership
	{
		Owner,
		Reference
	};

	template<typename T, size_t N, typename = CharType<T>>
	static Data from_bytes(const T (&bytes)[N])
	{
		return Data(bytes, N, Ownership::Reference);
	}

	template<typename T, size_t N, typename = CharType<T>>
	static Data from_literal(const T (&literal)[N])
	{
		return Data(literal, N - 1, Ownership::Reference);
	}

	static Data load_asset(const char *const asset);
	static Data load_document(const char *const document);

	/// Constructs an empty data object. No memory will be allocated.
	Data()
	    : ownership_(Ownership::Owner), allocated_(0), sz_(0), data_(nullptr) {}

	Data(Data&& d)
	    : ownership_(d.ownership_), allocated_(d.allocated_), sz_(d.sz_),
	      data_(d.data_)
	{
		d.allocated_ = 0;
		d.sz_ = 0;
		d.data_ = nullptr;
	}

	/// Constructs a data object with the contents of the file.
	explicit Data(const File &);

	/// Constructs a wrapper around a buffer.
	Data(const void *buffer, const size_t size, const Ownership ownership)
	    : ownership_(ownership), allocated_(size), sz_(size),
	      data_(const_cast<void*>(buffer)) {}

	~Data();

	/// Returns raw byte array.
	/// \return Pointer to array. Returns \c nullptr if buffer is empty.
	void* bytes() const { return data_; }

	/// Saves data to file.
	/// \return \c true on success, \c false otherwise.
	bool save(const char *const path) const;

	/// Returns the size of this buffer.
	size_t size() const { return sz_; }

	explicit operator bool() const { return data_; }
	operator void*() const { return data_; }
	operator char*() const { return static_cast<char*>(data_); }

	operator unsigned char*() const
	{
		return static_cast<unsigned char*>(data_);
	}

#ifdef RAINBOW_OS_IOS
	operator NSData*() const
	{
		return [NSData dataWithBytesNoCopy:data_ length:sz_ freeWhenDone:NO];
	}

	operator NSMutableData*() const
	{
		return [NSMutableData dataWithBytesNoCopy:data_
		                                   length:sz_
		                             freeWhenDone:NO];
	}
#endif

private:
	Ownership ownership_;  ///< Decides whether to free the buffer on destruction.
	size_t allocated_;     ///< Allocated memory size.
	size_t sz_;            ///< Size of used buffer, not necessarily equal to allocated.
	void *data_;           ///< Actual buffer, implemented as a C-array.

	/// Resizes allocated memory segment. If the requested allocation size is
	/// smaller than current allocated size, nothing will happen.
	void allocate(const size_t size);
};

#endif  // DATA_H_
