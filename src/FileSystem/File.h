// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef FILESYSTEM_FILE_H_
#define FILESYSTEM_FILE_H_

#include <climits>
#include <cstdint>
#include <cstdio>
#include <functional>
#include <sys/stat.h>

#include "Common/Data.h"
#include "Common/Logging.h"
#include "Common/NonCopyable.h"
#include "Platform/Macros.h"

#ifdef RAINBOW_OS_ANDROID
#    define CONSTEXPR
#else
#    define CONSTEXPR constexpr
#endif

#ifndef RAINBOW_OS_WINDOWS
#    define _fileno fileno
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
        constexpr auto file_access_mode(FileType type)
        {
            switch (type)
            {
                case FileType::UserFile:
                    return "w+b";
                default:
                    return "rb";
            }
        }

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
                case SeekOrigin::Current:
                    return SEEK_CUR;
                case SeekOrigin::End:
                    return SEEK_END;
                default:
                    return SEEK_SET;
            }
        }
    }  // namespace detail

    template <typename T>
    class TFile : protected T, private NonCopyable<T>
    {
    public:
        static auto open(czstring path, FileType file_type) -> TFile
        {
            R_ASSERT(file_type != FileType::UserFile,
                     "Invalid file type specified for read-only file");

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
                fclose(T::handle());
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

                struct stat file_status;
                const int fd = _fileno(T::handle());
                return fstat(fd, &file_status) != 0 ? kInvalidFileSize
                                                    : file_status.st_size;
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

                return fread(dst, sizeof(uint8_t), size, T::handle());
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

                auto stream = T::handle();
                const int origin = detail::seek_origin(seek_origin);
                return fseek(stream, offset, origin) == 0;
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

                auto stream = T::handle();
                return ftell(stream);
            }
        }

        /// <summary>
        ///   Returns whether this instance has an associated file.
        /// </summary>
        explicit operator bool() const { return T::operator bool(); }

        explicit operator FILE*() const
        {
            if CONSTEXPR(T::is_platform_handle())
                return nullptr;
            else
                return T::handle();
        }

#if defined(RAINBOW_OS_MACOS) && defined(USE_HEIMDALL)
        static auto read(czstring path) -> Data
        {
            czstring access_mode = detail::file_access_mode(FileType::Asset);
            TFile file;
            file.set_handle(fopen(path, access_mode));
            return read(file);
        }
#endif

    protected:
        template <typename U>
        static auto open(czstring path, FileType file_type) -> U
        {
            U file;
            if (path != nullptr && path[0] != '\0')
            {
                const auto real_path = T::resolve_path(path, file_type);
                if (!T::open(real_path.c_str(), file_type, std::ref(file)))
                {
                    czstring mode = detail::file_access_mode(file_type);
                    file.set_handle(fopen(real_path.c_str(), mode));
                }
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
    };

    template <typename T>
    class TWriteableFile : public TFile<T>
    {
        using base = TFile<T>;

    public:
        static auto open(czstring path) -> TWriteableFile
        {
            return base::template open<TWriteableFile>(
                path, FileType::UserFile);
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

                return fwrite(buffer, sizeof(uint8_t), size, T::handle());
            }
        }
    };
}  // namespace rainbow

#undef CONSTEXPR

#ifdef RAINBOW_OS_ANDROID
#    include "FileSystem/impl/File.android.h"
#endif
#include "FileSystem/impl/File.system.h"

#ifndef RAINBOW_OS_WINDOWS
#    undef _fileno
#endif

#ifdef USE_PLATFORM_FILE
#    undef USE_PLATFORM_FILE
#endif

#endif
