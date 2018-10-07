// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_DATA_H_
#define COMMON_DATA_H_

#include <cstddef>
#include <cstdint>

#include "Common/NonCopyable.h"
#include "Common/String.h"
#include "Platform/Macros.h"

namespace rainbow
{
    /// <summary>Wrapper for byte buffers.</summary>
    /// <remarks>
    ///   <para>
    ///     Data objects are used to read from disk into memory or to save files
    ///     to disk.
    ///   </para>
    ///   <para>
    ///     Data is "toll-free bridged" with its platform-specific counterparts.
    ///     On iOS, it is safe to perform a static cast of this type to an
    ///     <c>NSData</c> or <c>NSMutableData</c> (which in turn can be cast to
    ///     a <c>CFData</c> or <c>CFMutableData</c>). On other platforms, it can
    ///     be casted to a <c>void*</c> or a <c>uint8_t*</c>.
    ///   </para>
    /// </remarks>
    class Data : private NonCopyable<Data>
    {
    public:
        enum class Ownership
        {
            Owner,
            Reference
        };

        template <typename T, size_t N>
        static auto from_bytes(const T (&bytes)[N])
        {
            if (std::is_same_v<std::make_unsigned_t<std::decay_t<T>>, uint8_t>)
                return Data{bytes, N, Ownership::Reference};
        }

        template <typename T, size_t N>
        static auto from_literal(const T (&literal)[N])
        {
            if (std::is_same_v<std::make_unsigned_t<std::decay_t<T>>, uint8_t>)
                return Data{literal, N - 1, Ownership::Reference};
        }

        /// <summary>
        ///   Constructs an empty data object. No memory will be allocated.
        /// </summary>
        Data()
            : ownership_(Ownership::Owner), allocated_(0), sz_(0),
              data_(nullptr)
        {
        }

        Data(Data&& d)
            : ownership_(d.ownership_), allocated_(d.allocated_), sz_(d.sz_),
              data_(d.data_)
        {
            d.allocated_ = 0;
            d.sz_ = 0;
            d.data_ = nullptr;
        }

        /// <summary>Constructs a wrapper around a buffer.</summary>
        Data(const void* buffer, size_t size, Ownership ownership)
            : ownership_(ownership), allocated_(size), sz_(size),
              data_(const_cast<void*>(buffer))
        {
        }

        ~Data();

        template <typename T>
        auto as() const
        {
            return static_cast<const T>(data_);
        }

        /// <summary>Returns raw byte array.</summary>
        /// <returns>
        ///   Pointer to array. Returns <c>nullptr</c> if buffer is empty.
        /// </returns>
        auto bytes() const { return as<uint8_t*>(); }

        /// <summary>Returns the size of this buffer.</summary>
        auto size() const { return sz_; }

        explicit operator bool() const { return data_; }

#ifdef RAINBOW_OS_IOS
        operator NSData*() const
        {
            return [NSData dataWithBytesNoCopy:data_
                                        length:sz_
                                  freeWhenDone:NO];
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
        void* data_;           ///< Actual buffer, implemented as a C-array.
    };
}

#endif
