#include "Platform/Macros.h"
#if defined(RAINBOW_OS_IOS) || defined(RAINBOW_OS_UNIX)

#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "Common/Debug.h"
#include "Common/IO.h"
#include "Common/impl/DataMap_Unix.h"

namespace Rainbow
{
	DataMapUnix::DataMapUnix(const char *const file) :
		len(0), off(0), addr(nullptr)
	{
		Rainbow::IO::FileHandle fh =
				Rainbow::IO::find_and_open(file, Rainbow::IO::kIOTypeAsset);
		if (!fh)
		{
			R_ERROR("[Rainbow] Failed to open '%s'\n", file);
			return;
		}
		this->len = Rainbow::IO::size(fh);
		if (!this->len)
			R_ERROR("[Rainbow] Failed to read '%s'\n", file);
		else
		{
			const int fd = fileno(fh);
		#if defined(RAINBOW_OS_IOS) || defined(RAINBOW_OS_MACOS)
			fcntl(fd, F_NOCACHE, 1);
			fcntl(fd, F_RDAHEAD, 1);
		#endif
			this->addr = mmap(nullptr, this->len, PROT_READ, MAP_PRIVATE, fd, 0);
			if (this->addr == MAP_FAILED)
			{
				R_ERROR(kErrorMapFailed, file, errno);
				this->addr = nullptr;
			}
		}
		Rainbow::IO::close(fh);
	}

	DataMapUnix::~DataMapUnix()
	{
		if (this->addr)
			munmap(this->addr, this->len);
	}
}

#endif
