// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_DATAMAP_H_
#define COMMON_DATAMAP_H_

#include <utility>

#include "Common/NonCopyable.h"
#include "Platform/Macros.h"

class Path;

namespace rainbow
{
    using byte_t = unsigned char;

    // TODO: Move error strings to a separate header.
    static constexpr char kErrorFileOpen[] = "Failed to open '%s' (%x)";
    static constexpr char kErrorFileRead[] = "Failed to read '%s' (%x)";
    static constexpr char kErrorMemoryMap[] = "Failed to memory map '%s' (%x)";

    template <typename T>
    class TDataMap : private T, private NonCopyable<TDataMap<T>>
    {
    public:
#ifndef RAINBOW_OS_ANDROID
        template <size_t N>
        TDataMap(const byte_t (&bytes)[N]) : T(bytes) {}
#endif

        explicit TDataMap(const Path& path) : T(path) {}
        TDataMap(TDataMap&& data) : T(std::move(data)) {}

        /// <summary>Returns offset raw byte array.</summary>
        /// <returns>
        ///   Pointer to array. Returns <c>nullptr</c> if buffer is empty.
        /// </returns>
        const byte_t* data() const { return T::data(); }

        /// <summary>Offsets data map's start address.</summary>
        void offset(size_t offset) { return T::offset(offset); }

        /// <summary>Returns offset buffer size.</summary>
        size_t size() const { return T::size(); }

        explicit operator bool() const { return T::operator bool(); }
    };
}

#if defined(RAINBOW_OS_ANDROID)
#   include "Common/impl/DataMap_Android.h"
#elif defined(RAINBOW_OS_WINDOWS)
#   include "Common/impl/DataMap_Win.h"
#else
#   include "Common/impl/DataMap_Unix.h"
#endif

#endif
