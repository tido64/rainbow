// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef FILESYSTEM_FILE_H_
#define FILESYSTEM_FILE_H_

#include <climits>
#include <cstdint>
#include <functional>

#include <physfs.h>

#include "Common/Data.h"
#include "Common/Logging.h"
#include "Common/NonCopyable.h"
#include "Platform/Macros.h"

#ifdef RAINBOW_OS_ANDROID
#    define CONSTEXPR
#else
#    define CONSTEXPR constexpr
#endif

namespace rainbow
{
    constexpr size_t kInvalidFileSize = std::numeric_limits<size_t>::max();

    enum class FileType
    {
        Asset,
        UserAsset,
        UserFile,
    };

    enum class SeekOrigin
    {
        Set,
        Current,
        End,
    };

    namespace detail
    {
        constexpr auto seek_origin(int origin)
        {
            switch (origin)
            {
                case SEEK_CUR:
                    return SeekOrigin::Current;
                case SEEK_END:
                    return SeekOrigin::End;
                default:
                    return SeekOrigin::Set;
            }
        }

        constexpr auto seek_origin(SeekOrigin origin)
        {
            switch (origin)
            {
                case SeekOrigin::Set:
                    return SEEK_SET;
                case SeekOrigin::Current:
                    return SEEK_CUR;
                case SeekOrigin::End:
                    return SEEK_END;
                default:
                    UNREACHABLE();
            }
        }
    }  // namespace detail

    template <typename T>
    class TFile : protected T, private NonCopyable<T>
    {
    public:
        static auto open(czstring path, FileType file_type) -> TFile
        {
            return open<TFile>(path, file_type);
        }

        static auto read(czstring path, FileType file_type) -> Data
        {
            const auto& file = open(path, file_type);
            if (!file)
            {
                LOGW("No such file: %s", path);
                return {};
            }

            return read(file);
        }

        constexpr TFile() = default;
        TFile(TFile&& file) noexcept : T(std::move(file)) {}

        ~TFile()
        {
            if (!*this)
                return;

            if CONSTEXPR (T::is_platform_handle())
                T::close();
            else
                PHYSFS_close(T::handle());
        }

        /// <summary>Returns the file size; -1 on error.</summary>
        auto size() const -> size_t
        {
            if CONSTEXPR (T::is_platform_handle())
            {
                return T::size();
            }
            else
            {
                R_ASSERT(T::handle() != nullptr, "No file handle");

                return PHYSFS_fileLength(T::handle());
            }
        }

        /// <summary>
        ///   Reads <paramref name="size"/> bytes from file into buffer
        ///   <paramref name="dst"/>.
        /// </summary>
        /// <param name="dst">[out] Destination buffer.</param>
        /// <param name="size">Number of bytes to read.</param>
        /// <returns>Number of bytes read.</returns>
        auto read(void* dst, size_t size) const -> size_t
        {
            if CONSTEXPR (T::is_platform_handle())
            {
                return T::read(dst, size);
            }
            else
            {
                R_ASSERT(T::handle() != nullptr, "No file handle");

                return PHYSFS_readBytes(T::handle(), dst, size);
            }
        }

        /// <summary>
        ///   Sets the file position indicator for the file stream to the value
        ///   pointed to by <paramref name="offset"/>.
        ///   <seealso cref="fseek"/>
        /// </summary>
        /// <param name="offset">
        ///   Number of bytes to shift the position relative to start.
        /// </param>
        /// <returns><c>true</c> upon success; <c>false</c> otherwise.</returns>
        bool seek(int64_t offset) const
        {
            return seek(offset, SeekOrigin::Set);
        }

        /// <summary>
        ///   Sets the file position indicator for the file stream to the value
        ///   pointed to by <paramref name="offset"/>.
        ///   <seealso cref="fseek"/>
        /// </summary>
        /// <param name="offset">
        ///   Number of bytes to shift the position relative to origin.
        /// </param>
        /// <param name="origin">Position to which offset is added.</param>
        /// <returns><c>true</c> upon success; <c>false</c> otherwise.</returns>
        bool seek(int64_t offset, SeekOrigin seek_origin) const
        {
            if CONSTEXPR (T::is_platform_handle())
            {
                return T::seek(offset, seek_origin);
            }
            else
            {
                R_ASSERT(T::handle() != nullptr, "No file handle");

                const auto pos = absolute_position(offset, seek_origin);
                return PHYSFS_seek(T::handle(), pos) != 0;
            }
        }

        /// <summary>Returns the current file position indicator.</summary>
        auto tell() const -> size_t
        {
            if CONSTEXPR (T::is_platform_handle())
            {
                return T::tell();
            }
            else
            {
                R_ASSERT(T::handle() != nullptr, "No file handle");

                return PHYSFS_tell(T::handle());
            }
        }

        /// <summary>
        ///   Returns whether this instance has an associated file.
        /// </summary>
        explicit operator bool() const { return T::operator bool(); }

    protected:
        template <typename U>
        static auto open(czstring path, FileType file_type) -> U
        {
            U file;
            if (!is_empty(path))
            {
                const auto real_path = T::resolve_path(path, file_type);
                if (!T::open(real_path.c_str(), file_type, std::ref(file)))
                    file.set_handle(PHYSFS_openRead(real_path.c_str()));
            }

            return file;
        }

        static auto read(const TFile& file) -> Data
        {
            const size_t size = file.size();
            if (size == 0)
                return {};

            auto buffer = std::make_unique<uint8_t[]>(size + 1);
            if (file.read(buffer.get(), size) != size)
                return {};

            // Ensure the buffer is null-terminated so it also can be used as a
            // string without copying.
            buffer[size] = 0;
            return {buffer.release(), size, Data::Ownership::Owner};
        }

    private:
        auto absolute_position(int64_t offset, SeekOrigin seek_origin) const
            -> size_t
        {
            switch (seek_origin)
            {
                case SeekOrigin::Set:
                    return offset;
                case SeekOrigin::Current:
                    return tell() + offset;
                case SeekOrigin::End:
                    return size() - offset;
                default:
                    UNREACHABLE();
            }
        }
    };

    template <typename T>
    class TWriteableFile : private TFile<T>
    {
        using base = TFile<T>;

    public:
        static auto open(czstring path) -> TWriteableFile
        {
            TWriteableFile file;
            if (!is_empty(path))
            {
                constexpr auto file_type = FileType::UserFile;
                const auto real_path = T::resolve_path(path, file_type);
                if (!T::open(real_path.c_str(), file_type, std::ref(file)))
                    file.set_handle(PHYSFS_openWrite(real_path.c_str()));
            }

            return file;
        }

        static auto write(czstring path, const Data& data) -> size_t
        {
            auto file = open(path);
            return !file ? 0 : file.write(data.bytes(), data.size());
        }

        constexpr TWriteableFile() = default;

        TWriteableFile(TWriteableFile&& file) noexcept : base(std::move(file))
        {
        }

        using base::seek;
        using base::size;
        using base::tell;
        using base::operator bool;

        auto write(const void* buffer, size_t size) const -> size_t
        {
            R_ASSERT(size > 0, "No data to write");

            if CONSTEXPR (T::is_platform_handle())
            {
                return T::write(buffer, size);
            }
            else
            {
                R_ASSERT(T::handle() != nullptr, "No file handle");

                return PHYSFS_writeBytes(T::handle(), buffer, size);
            }
        }
    };
}  // namespace rainbow

#undef CONSTEXPR

#include "FileSystem/File.system.h"
#ifdef RAINBOW_OS_ANDROID
#    include "FileSystem/File.android.h"
#else
namespace rainbow
{
    using File = TFile<system::File>;
}  // namespace rainbow
#endif

#endif
