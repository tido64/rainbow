// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_DATAMAP_H_
#define COMMON_DATAMAP_H_

#include <cstddef>
#include <cstdint>
#include <utility>

#include "Common/NonCopyable.h"
#include "Platform/Macros.h"

#define kErrorFileOpen   "Failed to open '%s' (%x)"
#define kErrorFileRead   "Failed to read '%s' (%x)"
#define kErrorMemoryMap  "Failed to memory map '%s' (%x)"

namespace rainbow
{
    namespace filesystem { class Path; }

    template <typename T>
    class TDataMap : private T, private NonCopyable<TDataMap<T>>
    {
    public:
#ifndef RAINBOW_OS_ANDROID
        template <size_t N>
        TDataMap(const uint8_t (&bytes)[N]) : T(bytes) {}
#endif

        explicit TDataMap(const filesystem::Path& path) : T(path) {}
        TDataMap(TDataMap&& data) : T(std::move(data)) {}

        /// <summary>Returns offset raw byte array.</summary>
        /// <returns>
        ///   Pointer to array. Returns <c>nullptr</c> if buffer is empty.
        /// </returns>
        auto data() const -> const uint8_t* { return T::data(); }

        /// <summary>Offsets data map's start address.</summary>
        void offset(size_t offset) { return T::offset(offset); }

        /// <summary>Returns offset buffer size.</summary>
        auto size() const -> size_t { return T::size(); }

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
