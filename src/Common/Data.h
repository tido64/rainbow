#ifndef DATA_H_
#define DATA_H_

#include "Platform.h"

#define RAINBOW_DATA_ASSETS_PATH        "Data/"
#define RAINBOW_DATA_ASSETS_PATH_LENGTH 6
#define RAINBOW_DATA_USER_PATH          "./"
#define RAINBOW_DATA_USER_PATH_LENGTH   3

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
	/// Free memory allocated by get_path().
	static void free(const void *const data);

	/// Convenience method for getting the full path to a file.
	/// \param file       The file to obtain full path for.
	/// \param user_data  Whether to get path to user data.
	/// \return Full path to file. Returned string must be freed with Data::free.
	static const char* get_path(const char *const file = 0, const bool user_data = false);

	/// Construct an empty data object. No memory will be allocated.
	Data();

	/// Construct a data object with the contents of the file.
	explicit Data(const char *const file);

	~Data();

	/// Return raw byte array.
	/// \return Pointer to array. Returns 0 if buffer is empty.
	unsigned char* bytes() const;

	bool copy(const void *const data, const unsigned int length);

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
	unsigned int allocated;  ///< Allocated memory size
	unsigned int sz;         ///< Size of used buffer, not necessarily equal to allocated
	unsigned char *data;     ///< Actual buffer, implemented as a C-array

	/// Not sure whether we'll need this. So it'll stay undefined for now.
	Data(const Data &);

	/// Resize allocated memory segment. If the requested allocation size is
	/// smaller than current allocated size, nothing will happen.
	/// \return True on successful allocation. False otherwise.
	bool allocate(const unsigned int size);

#endif

};

#include "Common/impl/Data.inl"

#endif
