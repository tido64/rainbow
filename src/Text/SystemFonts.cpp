// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Text/SystemFonts.h"

#include <sys/stat.h>

#include "Common/NonCopyable.h"
#include "Platform/Macros.h"
#if defined(RAINBOW_OS_UNIX)
#    include "Common/Logging.h"
#    include "FileSystem/FileSystem.h"
#elif defined(RAINBOW_OS_WINDOWS)
#    define fileno _fileno
#endif

using rainbow::czstring;
using rainbow::Data;

namespace
{
    class MonospaceFont : private rainbow::NonCopyable<MonospaceFont>
    {
    public:
        MonospaceFont()
            : file_(fopen(rainbow::text::monospace_font_path(), "rb"))
        {
        }

        ~MonospaceFont() { fclose(file_); }

        [[nodiscard]] auto file_size() const -> size_t
        {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init)
            struct stat file_status;
            const int fd = fileno(file_);
            return fstat(fd, &file_status) != 0 ? 0 : file_status.st_size;
        }

        operator FILE*() const { return file_; }

    private:
        FILE* file_;
    };
}  // namespace

auto rainbow::text::monospace_font() -> Data
{
    MonospaceFont font;

    const auto size = font.file_size();
    if (size == 0)
        return {};

    auto buffer = std::make_unique<uint8_t[]>(size);
    if (fread(buffer.get(), sizeof(uint8_t), size, font) != size)
        return {};

    return {buffer.release(), size, Data::Ownership::Owner};
}

auto rainbow::text::monospace_font_path() -> czstring
{
#if defined(RAINBOW_OS_ANDROID)
    return "/system/fonts/DroidSansMono.ttf";
#elif defined(RAINBOW_OS_MACOS)
    return "/System/Library/Fonts/Menlo.ttc";
#elif defined(RAINBOW_OS_UNIX)
    constexpr const char* const fonts[]{
        "/usr/share/fonts/opentype/fira/FiraMono-Regular.otf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf",
        "/usr/share/fonts/TTF/DejaVuSansMono.ttf",
    };
    for (auto path : fonts)
    {
        if (system::is_regular_file(path))
            return path;
    }

    R_ABORT("No monospace fonts found");
    return nullptr;
#elif defined(RAINBOW_OS_WINDOWS)
    return "C:/Windows/Fonts/consola.ttf";
#else
    return nullptr;
#endif
}
