#ifndef DATA_H_
#define DATA_H_

#include <cstddef>

#include "Platform.h"

#define RAINBOW_PATH_LENGTH 512  ///< Max string length, including null-terminator.

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
/// Copyright 2011-12 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen
class Data
{
public:
	/// Free memory allocated by Data::get_path().
	static void free(const void *const data);

	/// Convenience method for getting the full path to a data file.
	/// \param file  The file to obtain full path for. If \c nullptr, the path
	///              is returned.
	/// \return Full path to file. Returned string must be freed with
	///         Data::free() unless \p file was a \c nullptr.
	static const char* get_path(const char *const file = nullptr);

	/// Set the path where all data files reside.
	/// \param path  Path to data files.
	/// \return Path string.
	static const char* set_datapath(const char *const path);

	/// Set the path where all user files reside.
	/// \param path  Path to user files.
	/// \return Path string.
	static const char* set_userdatapath(const char *const path);

	/// Construct an empty data object. No memory will be allocated.
	inline Data();

	/// Construct a data object with the contents of the file.
	explicit Data(const char *const file);

	~Data();

	/// Return raw byte array.
	/// \return Pointer to array. Returns 0 if buffer is empty.
	inline unsigned char* bytes() const;

	/// Make full copy of data set.
	/// \param data    Data to take make copy of.
	/// \param length  Length of data.
	void copy(const void *const data, const size_t length);

	/// Save data to file.
	/// \return \c true on success, \c false otherwise.
	bool save(const char *const file) const;

	/// Take ownership of data set.
	/// \param data    Data to take ownership of.
	/// \param length  Length of data.
	void set(unsigned char *data, const size_t length);

	/// Return the size of this buffer.
	inline size_t size() const;

	inline operator bool() const;
	inline operator void*() const;
	inline operator char*() const;
	inline operator unsigned char*() const;

#ifdef RAINBOW_IOS

	inline operator NSData*() const;
	inline operator NSMutableData*() const;

private:
	NSMutableData *data;

#else

private:
#ifdef RAINBOW_ANDROID
	static struct AAssetManager *asset_manager;      ///< Android asset manager.
#else
	static size_t data_path_length;                  ///< Length of data path string.
	static size_t userdata_path_length;              ///< Length of user path string.
	static char data_path[RAINBOW_PATH_LENGTH];      ///< Path to data files.
	static char userdata_path[RAINBOW_PATH_LENGTH];  ///< Path to user files.
#endif

	size_t allocated;     ///< Allocated memory size.
	size_t sz;            ///< Size of used buffer, not necessarily equal to allocated.
	unsigned char *data;  ///< Actual buffer, implemented as a C-array.

	/// Resize allocated memory segment. If the requested allocation size is
	/// smaller than current allocated size, nothing will happen.
	void allocate(const size_t size);

#endif

	/// Intentionally left undefined.
	Data(const Data &);

	/// Intentionally left undefined.
	Data& operator=(const Data &);
};

#include "Common/impl/Data-inl.h"

#endif
