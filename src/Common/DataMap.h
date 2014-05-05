// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_DATAMAP_H_
#define COMMON_DATAMAP_H_

#include "Common/NonCopyable.h"
#include "Platform/Macros.h"

class Path;

namespace Rainbow
{
	// TODO: Move error strings to a separate header.
	static const char kErrorMapFailed[] =
	    "[Rainbow] Failed to memory map '%s' (%x)\n";

	template<typename T>
	class DataMapBase : private T, private NonCopyable<DataMapBase<T>>
	{
	public:
		explicit DataMapBase(const Path &path);

		/// Returns offset raw byte array.
		/// \return Pointer to array. Returns \c nullptr if buffer is empty.
		const unsigned char* bytes() const;

		/// Offsets data map's start address.
		void offset(const size_t offset);

		/// Returns offset buffer size.
		size_t size() const;

		explicit operator bool() const;
		operator const void*() const;
		operator const char*() const;
		operator const unsigned char*() const;
	};

	template<typename T>
	DataMapBase<T>::DataMapBase(const Path &path) : T(path) { }

	template<typename T>
	const unsigned char* DataMapBase<T>::bytes() const
	{
		return T::bytes();
	}

	template<typename T>
	void DataMapBase<T>::offset(const size_t offset)
	{
		return T::offset(offset);
	}

	template<typename T>
	size_t DataMapBase<T>::size() const
	{
		return T::size();
	}

	template<typename T>
	DataMapBase<T>::operator bool() const
	{
		return T::operator bool();
	}

	template<typename T>
	DataMapBase<T>::operator const void*() const
	{
		return T::operator const void*();
	}

	template<typename T>
	DataMapBase<T>::operator const char*() const
	{
		return T::operator const char*();
	}

	template<typename T>
	DataMapBase<T>::operator const unsigned char*() const
	{
		return T::operator const unsigned char*();
	}
}

#if defined(RAINBOW_OS_ANDROID)
#	include "Common/impl/DataMap_Android.h"
#elif defined(RAINBOW_OS_WINDOWS)
#	include "Common/impl/DataMap_Win.h"
#else
#	include "Common/impl/DataMap_Unix.h"
#endif

#endif
