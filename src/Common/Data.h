// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_DATA_H_
#define COMMON_DATA_H_

#include <cstddef>
#include <cstdint>
#include <type_traits>

#include "Common/NonCopyable.h"
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
            Reference,
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
        Data() = default;

        Data(Data&& d) noexcept
            : data_(d.data_), size_(d.size_), ownership_(d.ownership_)
        {
            d.data_ = nullptr;
            d.size_ = 0;
        }

        /// <summary>Constructs a wrapper around a buffer.</summary>
        Data(const void* buffer, size_t size, Ownership ownership)
            // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
            : data_(const_cast<void*>(buffer)), size_(size),
              ownership_(ownership)
        {
        }

        ~Data()
        {
            if (ownership_ != Ownership::Owner)
                return;

            operator delete(data_);
        }

        template <typename T>
        [[nodiscard]] auto as() const
        {
            return static_cast<const T>(data_);
        }

        /// <summary>Returns raw byte array.</summary>
        /// <returns>
        ///   Pointer to array. Returns <c>nullptr</c> if buffer is empty.
        /// </returns>
        [[nodiscard]] auto bytes() const { return as<uint8_t*>(); }

        /// <summary>Returns the size of this buffer.</summary>
        [[nodiscard]] auto size() const { return size_; }

        explicit operator bool() const { return data_ != nullptr; }

#ifdef RAINBOW_OS_IOS
        operator NSData*() const
        {
            return [NSData dataWithBytesNoCopy:data_
                                        length:size_
                                  freeWhenDone:NO];
        }

        operator NSMutableData*() const
        {
            return [NSMutableData dataWithBytesNoCopy:data_
                                               length:size_
                                         freeWhenDone:NO];
        }
#endif

    private:
        /// <summary>Actual buffer, implemented as a C-array.</summary>
        void* data_ = nullptr;

        /// <summary>
        ///   Size of used buffer, not necessarily equal to allocated.
        /// </summary>
        size_t size_ = 0;

        /// <summary>
        ///   Whether the buffer should be freed on destruction.
        /// </summary>
        Ownership ownership_ = Ownership::Owner;
    };
}  // namespace rainbow

#endif
