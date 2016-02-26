// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Common/DataMap.h"

#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "Common/Logging.h"
#include "FileSystem/File.h"
#include "FileSystem/Path.h"

using rainbow::DataMapUnix;

DataMapUnix::DataMapUnix(const Path& path)
    : len_(0), off_(0), addr_(nullptr), is_embedded_(false)
{
    const File& f = File::open(path);
    if (!f)
    {
        LOGE(kErrorFileOpen, static_cast<const char*>(path), errno);
        return;
    }

    len_ = f.size();
    if (!len_)
        LOGE(kErrorFileRead, static_cast<const char*>(path), errno);
    else
    {
        const int fd = fileno(f);
#if defined(RAINBOW_OS_IOS) || defined(RAINBOW_OS_MACOS)
        fcntl(fd, F_NOCACHE, 1);
        fcntl(fd, F_RDAHEAD, 1);
#endif
        addr_ = mmap(nullptr, len_, PROT_READ, MAP_PRIVATE, fd, 0);
        if (addr_ == MAP_FAILED)
        {
            addr_ = nullptr;
            LOGE(kErrorMemoryMap, static_cast<const char*>(path), errno);
        }
    }
}

DataMapUnix::DataMapUnix(DataMapUnix&& data)
    : len_(data.len_), off_(data.off_), addr_(data.addr_),
      is_embedded_(data.is_embedded_)
{
    data.len_ = 0;
    data.off_ = 0;
    data.addr_ = nullptr;
}

DataMapUnix::~DataMapUnix()
{
    if (addr_ == nullptr || is_embedded_)
        return;

    munmap(addr_, len_);
}
