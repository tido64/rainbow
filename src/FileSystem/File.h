// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef FILESYSTEM_FILE_H_
#define FILESYSTEM_FILE_H_

#include <cstdint>
#include <cstdio>

#ifdef _MSC_VER
#   pragma warning(push)
#   pragma warning(disable: 4996)
#endif
#include <mapbox/variant.hpp>
#ifdef _MSC_VER
#   pragma warning(pop)
#endif

#include "Common/NonCopyable.h"

struct AAsset;

namespace rainbow
{
    namespace filesystem { class Path; }

    class IFile : private NonCopyable<IFile>
    {
    public:
        virtual ~IFile() {}

        /// <summary>Returns the file size.</summary>
        virtual auto size() const -> size_t = 0;

        /// <summary>
        ///   Reads <paramref name="size"/> bytes from file into buffer
        ///   <paramref name="dst"/>.
        /// </summary>
        /// <param name="dst">[out] Destination buffer.</param>
        /// <param name="size">Number of bytes to read.</param>
        /// <returns>Number of bytes read.</returns>
        virtual auto read(void* dst, size_t size) -> size_t = 0;

        /// <summary>
        ///   Sets the file position indicator for the file stream to the value
        ///   pointed to by <paramref name="offset"/>.
        ///   <seealso cref="fseek"/>
        /// </summary>
        /// <param name="offset">
        ///   Number of bytes to shift the position relative to origin.
        /// </param>
        /// <param name="origin">Position to which offset is added.</param>
        /// <returns>0 upon success, nonzero value otherwise.</returns>
        virtual auto seek(int64_t offset, int origin) -> int = 0;

        /// <summary>Writes <paramref name="buffer"/> to file.</summary>
        /// <param name="buffer">Source buffer.</param>
        /// <param name="size">Number of bytes to write.</param>
        /// <returns>Number of bytes written.</returns>
        virtual auto write(const void* buffer, size_t size) -> size_t = 0;

        /// <summary>Returns whether the file was successfully opened.</summary>
        virtual /*explicit*/ operator bool() const = 0;
    };

    class File final : private NonCopyable<File>
    {
    public:
        static auto open(const filesystem::Path& path) -> File;
        static auto open_asset(const char* path) -> File;
        static auto open_document(const char* path) -> File;
        static auto open_write(const char* path) -> File;

        File(File&& file) noexcept : handle_(std::move(file.handle_))
        {
            file.handle_ = nullptr;
        }

        ~File();

        /// <summary>
        ///   Returns whether this instance has an associated file.
        /// </summary>
        auto is_open() const -> bool;

        /// <summary>Returns the file size.</summary>
        auto size() const -> size_t;

        /// <summary>
        ///   Reads <paramref name="size"/> bytes from file into buffer
        ///   <paramref name="dst"/>.
        /// </summary>
        /// <param name="dst">[out] Destination buffer.</param>
        /// <param name="size">Number of bytes to read.</param>
        /// <returns>Number of bytes read.</returns>
        auto read(void* dst, size_t size) -> size_t;

        /// <summary>
        ///   Sets the file position indicator for the file stream to the value
        ///   pointed to by <paramref name="offset"/>.
        ///   <seealso cref="fseek"/>
        /// </summary>
        /// <param name="offset">
        ///   Number of bytes to shift the position relative to origin.
        /// </param>
        /// <param name="origin">Position to which offset is added.</param>
        /// <returns>0 upon success, nonzero value otherwise.</returns>
        auto seek(int64_t offset, int origin) -> int;

        /// <summary>
        ///   Writes buffer at <paramref name="buffer"/> to file.
        /// </summary>
        /// <param name="buffer">Source buffer.</param>
        /// <param name="size">Number of bytes to write.</param>
        /// <returns>Number of bytes written.</returns>
        auto write(const void* buffer, size_t size) -> size_t;

        explicit operator bool() const { return is_open(); }
        operator AAsset*() const;
        operator FILE*() const;

    private:
        mapbox::util::variant<std::nullptr_t, AAsset*, FILE*> handle_;

        explicit File(const filesystem::Path& path);
        File(const filesystem::Path& path, const char* mode);
    };
}

#endif
