#ifndef COMMON_DATAMAP_H_
#define COMMON_DATAMAP_H_

#include <cstddef>

#include "Common/NonCopyable.h"

namespace Rainbow
{
	// TODO: Move error strings to a separate header.
	static const char kErrorMapFailed[] = "[Rainbow] Failed to memory map '%s' (%x)\n";

	template<class T>
	class DataMapBase : private T, private NonCopyable<DataMapBase<T> >
	{
	public:
		inline DataMapBase(const char *const file);

		/// Returns offset raw byte array.
		/// \return Pointer to array. Returns \c nullptr if buffer is empty.
		inline const unsigned char* bytes() const;

		/// Offsets data map's start address.
		inline void offset(const size_t offset);

		/// Returns offset buffer size.
		inline size_t size() const;

		inline operator bool() const;
		inline operator const void*() const;
		inline operator const char*() const;
		inline operator const unsigned char*() const;
	};

	template<class T>
	DataMapBase<T>::DataMapBase(const char *const file) : T(file) { }

	template<class T>
	const unsigned char* DataMapBase<T>::bytes() const
	{
		return T::bytes();
	}

	template<class T>
	void DataMapBase<T>::offset(const size_t offset)
	{
		return T::offset(offset);
	}

	template<class T>
	size_t DataMapBase<T>::size() const
	{
		return T::size();
	}

	template<class T>
	DataMapBase<T>::operator bool() const
	{
		return T::operator bool();
	}

	template<class T>
	DataMapBase<T>::operator const void*() const
	{
		return T::operator const void*();
	}

	template<class T>
	DataMapBase<T>::operator const char*() const
	{
		return T::operator const char*();
	}

	template<class T>
	DataMapBase<T>::operator const unsigned char*() const
	{
		return T::operator const unsigned char*();
	}
}

#include "Platform/Macros.h"
#if defined(RAINBOW_ANDROID)
#	include "Common/impl/DataMap_Android.h"
#elif defined(RAINBOW_WIN)
#	include "Common/impl/DataMap_Win.h"
#else
#	include "Common/impl/DataMap_Unix.h"
#endif

#endif
