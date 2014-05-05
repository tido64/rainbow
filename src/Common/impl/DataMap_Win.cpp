// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Common/DataMap.h"
#ifdef RAINBOW_OS_WINDOWS

#include <io.h>
#include <windows.h>

#include "Common/Debug.h"
#include "FileSystem/Path.h"

namespace Rainbow
{
	DataMapWin::DataMapWin(const Path &path)
	    : len(0), off(0), addr(nullptr), handle(nullptr)
	{
		HANDLE fh = CreateFile(
				path, GENERIC_READ, 0, nullptr, OPEN_EXISTING,
				FILE_ATTRIBUTE_READONLY | FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
		if (fh == INVALID_HANDLE_VALUE)
		{
			R_ERROR("[Rainbow] Failed to open '%s' (%x)\n",
			        static_cast<const char*>(path), GetLastError());
			return;
		}
		LARGE_INTEGER size;
		if (!GetFileSizeEx(fh, &size))
			R_ERROR("[Rainbow] Failed to read '%s' (%x)\n",
			        static_cast<const char*>(path), GetLastError());
		else
		{
			this->handle = CreateFileMapping(fh, nullptr, PAGE_READONLY, 0, 0, nullptr);
			if (!this->handle)
			{
				R_ERROR("[Rainbow] Failed to create file mapping for '%s' (%x)\n",
				        static_cast<const char*>(path), GetLastError());
			}
			else
			{
				this->len = size.QuadPart;
				this->addr = MapViewOfFile(this->handle, FILE_MAP_READ, 0, 0, 0);
				if (!this->addr)
					R_ERROR(kErrorMapFailed, static_cast<const char*>(path), GetLastError());
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
