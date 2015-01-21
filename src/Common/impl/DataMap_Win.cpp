// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Common/DataMap.h"
#ifdef RAINBOW_OS_WINDOWS

#include <io.h>
#include <windows.h>

#include "Common/Logging.h"
#include "FileSystem/Path.h"

namespace rainbow
{
	DataMapWin::DataMapWin(const Path &path)
	    : len_(0), off_(0), addr_(nullptr), handle_(nullptr)
	{
		HANDLE fh = CreateFile(
		    path, GENERIC_READ, 0, nullptr, OPEN_EXISTING,
		    FILE_ATTRIBUTE_READONLY | FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
		if (fh == INVALID_HANDLE_VALUE)
		{
			LOGE(kErrorFileOpen,
			     static_cast<const char*>(path),
			     GetLastError());
			return;
		}
		LARGE_INTEGER size;
		if (!GetFileSizeEx(fh, &size))
		{
			LOGE(kErrorFileRead,
			     static_cast<const char*>(path),
			     GetLastError());
		}
		else
		{
			handle_ =
			    CreateFileMapping(fh, nullptr, PAGE_READONLY, 0, 0, nullptr);
			if (!handle_)
			{
				LOGE("Failed to create file mapping for '%s' (%x)",
				     static_cast<const char*>(path),
				     GetLastError());
			}
			else
			{
				len_ = size.QuadPart;
				addr_ = MapViewOfFile(handle_, FILE_MAP_READ, 0, 0, 0);
				if (!addr_)
				{
					LOGE(kErrorMemoryMap,
					     static_cast<const char*>(path),
					     GetLastError());
				}
			}
		}
		CloseHandle(fh);
	}

	DataMapWin::~DataMapWin()
	{
		if (addr_)
		{
			UnmapViewOfFile(addr_);
			CloseHandle(handle_);
		}
	}
}

#endif
