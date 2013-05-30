#include "Platform/Macros.h"
#ifdef RAINBOW_WIN

#include <io.h>
#include <windows.h>

#include "Common/Debug.h"
#include "Common/IO.h"
#include "Common/impl/DataMap_Win.h"

namespace Rainbow
{
	DataMapWin::DataMapWin(const char *const file) :
		len(0), off(0), addr(nullptr), handle(nullptr)
	{
		char path[256];
		Rainbow::IO::find(path, file, Rainbow::IO::kIOTypeAsset);
		R_ASSERT(path[0] != '\0', "Failed to find file");
		HANDLE fh = CreateFile(
				path, GENERIC_READ, 0, nullptr, OPEN_EXISTING,
				FILE_ATTRIBUTE_READONLY | FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
		if (fh == INVALID_HANDLE_VALUE)
		{
			R_ERROR("[Rainbow] Failed to open '%s' (%x)\n", file, GetLastError());
			return;
		}
		LARGE_INTEGER size;
		if (!GetFileSizeEx(fh, &size))
			R_ERROR("[Rainbow] Failed to read '%s' (%x)\n", file, GetLastError());
		else
		{
			this->handle = CreateFileMapping(fh, nullptr, PAGE_READONLY, 0, 0, nullptr);
			if (!this->handle)
			{
				R_ERROR("[Rainbow] Failed to create file mapping for '%s' (%x)\n",
				        file, GetLastError());
			}
			else
			{
				this->len = size.QuadPart;
				this->addr = MapViewOfFile(this->handle, FILE_MAP_READ, 0, 0, 0);
				if (!this->addr)
					R_ERROR(kErrorMapFailed, file, GetLastError());
			}
		}
		CloseHandle(fh);
	}

	DataMapWin::~DataMapWin()
	{
		if (this->addr)
		{
			UnmapViewOfFile(this->addr);
			CloseHandle(this->handle);
		}
	}
}

#endif
