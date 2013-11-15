#include "Platform/Macros.h"
#if defined(RAINBOW_OS_IOS) || defined(RAINBOW_OS_UNIX)

#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "Common/Debug.h"
#include "Common/impl/DataMap_Unix.h"
#include "FileSystem/File.h"
#include "FileSystem/Path.h"

namespace Rainbow
{
	DataMapUnix::DataMapUnix(const Path &path) :
		len(0), off(0), addr(nullptr)
	{
		const File &f = File::open(path);
		if (!f)
		{
			R_ERROR("[Rainbow] Failed to open '%s'\n", static_cast<const char*>(path));
			return;
		}
		this->len = f.size();
		if (!this->len)
			R_ERROR("[Rainbow] Failed to read '%s'\n", static_cast<const char*>(path));
		else
		{
			const int fd = fileno(f);
		#if defined(RAINBOW_OS_IOS) || defined(RAINBOW_OS_MACOS)
			fcntl(fd, F_NOCACHE, 1);
			fcntl(fd, F_RDAHEAD, 1);
		#endif
			this->addr = mmap(nullptr, this->len, PROT_READ, MAP_PRIVATE, fd, 0);
			if (this->addr == MAP_FAILED)
			{
				R_ERROR(kErrorMapFailed, static_cast<const char*>(path), errno);
				this->addr = nullptr;
			}
		}
	}

	DataMapUnix::~DataMapUnix()
	{
		if (this->addr)
			munmap(this->addr, this->len);
	}
}

#endif
