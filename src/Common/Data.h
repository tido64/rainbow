// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_DATA_H_
#define COMMON_DATA_H_

#include <type_traits>

#include "Common/NonCopyable.h"
#include "Platform/Macros.h"

class File;

template<typename T>
using EnableIfCharType = std::enable_if<
    std::is_same<typename std::make_unsigned<T>::type, unsigned char>::value>;

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

#ifdef _MSC_VER  // TODO: MSVC 2013 fails to deduce template argument.
	template<typename T, size_t N>
#else
	template<typename T, size_t N, typename Enable>
#endif
	static Data from_bytes(const T (&bytes)[N]);

#ifdef _MSC_VER  // TODO: MSVC 2013 fails to deduce template argument.
	template<typename T, size_t N>
#else
	template<typename T, size_t N, typename Enable>
#endif
	static Data from_literal(const T (&literal)[N]);

	static Data load_asset(const char *const asset);
	static Data load_document(const char *const document);

	/// Constructs an empty data object. No memory will be allocated.
	inline Data();

	inline Data(Data &&);

	/// Constructs a data object with the contents of the file.
	explicit Data(const File &);

	/// Constructs a wrapper around a buffer.
	inline Data(const void *buffer, const size_t size, const Ownership ownership);

	~Data();

	/// Returns raw byte array.
	/// \return Pointer to array. Returns \c nullptr if buffer is empty.
	inline void* bytes() const;

	/// Saves data to file.
	/// \return \c true on success, \c false otherwise.
	bool save(const char *const path) const;

	/// Returns the size of this buffer.
	inline size_t size() const;

	inline explicit operator bool() const;
	inline operator void*() const;
	inline operator char*() const;
	inline operator unsigned char*() const;

#ifdef RAINBOW_OS_IOS
	inline operator NSData*() const;
	inline operator NSMutableData*() const;
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

#ifdef _MSC_VER
template<typename T, size_t N>
#else
template<typename T, size_t N, typename = typename EnableIfCharType<T>::type>
#endif
Data Data::from_bytes(const T (&bytes)[N])
{
	return Data(bytes, N, Ownership::Reference);
}

#ifdef _MSC_VER
template<typename T, size_t N>
#else
template<typename T, size_t N, typename = typename EnableIfCharType<T>::type>
#endif
Data Data::from_literal(const T (&literal)[N])
{
	return Data(literal, N - 1, Ownership::Reference);
}

Data::Data()
    : ownership_(Ownership::Owner), allocated_(0), sz_(0), data_(nullptr) { }

Data::Data(Data &&d)
    : ownership_(d.ownership_), allocated_(d.allocated_), sz_(d.sz_),
      data_(d.data_)
{
	d.allocated_ = 0;
	d.sz_ = 0;
	d.data_ = nullptr;
}

Data::Data(const void *buffer, const size_t size, const Ownership ownership)
    : ownership_(ownership), allocated_(size), sz_(size),
      data_(const_cast<void*>(buffer)) { }

void* Data::bytes() const
{
	return data_;
}

size_t Data::size() const
{
	return sz_;
}

Data::operator bool() const
{
	return data_;
}

Data::operator void*() const
{
	return data_;
}

Data::operator char*() const
{
	return static_cast<char*>(data_);
}

Data::operator unsigned char*() const
{
	return static_cast<unsigned char*>(data_);
}

#ifdef RAINBOW_OS_IOS

Data::operator NSData*() const
{
	return [NSData dataWithBytesNoCopy:data_ length:sz_ freeWhenDone:NO];
}

Data::operator NSMutableData*() const
{
	return [NSMutableData dataWithBytesNoCopy:data_ length:sz_ freeWhenDone:NO];
}

#endif  // RAINBOW_OS_IOS
#endif  // DATA_H_
