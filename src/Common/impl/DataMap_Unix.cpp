// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Common/DataMap.h"
#if defined(RAINBOW_OS_IOS) || defined(RAINBOW_OS_UNIX)

#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "Common/Logging.h"
#include "FileSystem/File.h"
#include "FileSystem/Path.h"

namespace Rainbow
{
	DataMapUnix::DataMapUnix(const Path &path)
	    : len_(0), off_(0), addr_(nullptr)
	{
		const File &f = File::open(path);
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
				LOGE(kErrorMemoryMap, static_cast<const char*>(path), errno);
				addr_ = nullptr;
			}
		}
	}

	DataMapUnix::~DataMapUnix()
	{
		if (addr_)
			munmap(addr_, len_);
	}
}

#endif
