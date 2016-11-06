// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_DATA_H_
#define COMMON_DATA_H_

#include <cstddef>
#include <cstdint>

#include "Common/Constraints.h"
#include "Common/NonCopyable.h"
#include "Platform/Macros.h"

class File;

/// <summary>Wrapper for byte buffers.</summary>
/// <remarks>
///   <para>
///     Data objects are used to read from disk into memory or to save files to
///     disk.
///   </para>
///   <para>
///     Data is "toll-free bridged" with its platform-specific counterparts. On
///     iOS, it is safe to perform a static cast of this type to an
///     <c>NSData</c> or <c>NSMutableData</c> (which in turn can be cast to a
///     <c>CFData</c> or <c>CFMutableData</c>). On other platforms, it can be
///     casted to a <c>void*</c> or a <c>uint8_t*</c>.
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

    template <typename T, size_t N, typename = ByteType<T>>
    static auto from_bytes(const T (&bytes)[N])
    {
        return Data{bytes, N, Ownership::Reference};
    }

    template <typename T, size_t N, typename = ByteType<T>>
    static auto from_literal(const T (&literal)[N])
    {
        return Data{literal, N - 1, Ownership::Reference};
    }

    static auto load_asset(const char* asset) -> Data;
    static auto load_document(const char* document) -> Data;

    /// <summary>
    ///   Constructs an empty data object. No memory will be allocated.
    /// </summary>
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

    /// <summary>
    ///   Constructs a data object with the contents of the file.
    /// </summary>
    explicit Data(File&&);

    /// <summary>Constructs a wrapper around a buffer.</summary>
    Data(const void* buffer, size_t size, Ownership ownership)
        : ownership_(ownership), allocated_(size), sz_(size),
          data_(const_cast<void*>(buffer)) {}

    ~Data();

    /// <summary>Returns raw byte array.</summary>
    /// <returns>
    ///   Pointer to array. Returns <c>nullptr</c> if buffer is empty.
    /// </returns>
    auto bytes() const { return data_; }

    /// <summary>Saves data to file.</summary>
    /// <returns><c>true</c> on success, <c>false</c> otherwise.</returns>
    bool save(const char* path) const;

    /// <summary>Returns the size of this buffer.</summary>
    auto size() const { return sz_; }

    explicit operator bool() const { return data_; }
    operator void*() const { return data_; }
    operator char*() const { return static_cast<char*>(data_); }
    operator uint8_t*() const { return static_cast<uint8_t*>(data_); }

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
    void* data_;           ///< Actual buffer, implemented as a C-array.

    /// <summary>
    ///   Resizes allocated memory segment. If the requested allocation size is
    ///   smaller than current allocated size, nothing will happen.
    /// </summary>
    void allocate(size_t size);
};

#endif  // DATA_H_
