// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Common/DataMap.h"

#include <io.h>
#include <Windows.h>

#include "Common/Logging.h"
#include "FileSystem/Path.h"

using rainbow::DataMapWin;
using rainbow::filesystem::Path;

DataMapWin::DataMapWin(const Path& path)
    : len_(0), off_(0), addr_(nullptr), handle_(nullptr)
{
    HANDLE fh = CreateFile(  //
        path.c_str(),
        GENERIC_READ,
        0,
        nullptr,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_READONLY | FILE_FLAG_SEQUENTIAL_SCAN,
        nullptr);
    if (fh == INVALID_HANDLE_VALUE)
    {
        LOGE(kErrorFileOpen, path.c_str(), GetLastError());
        return;
    }

    LARGE_INTEGER size;
    if (!GetFileSizeEx(fh, &size))
    {
        LOGE(kErrorFileRead, path.c_str(), GetLastError());
    }
    else
    {
        handle_ = CreateFileMapping(fh, nullptr, PAGE_READONLY, 0, 0, nullptr);
        if (handle_ == nullptr)
        {
            LOGE("Failed to create file mapping for '%s' (%x)",
                 path.c_str(),
                 GetLastError());
        }
        else
        {
            len_ = size.QuadPart;
            addr_ = MapViewOfFile(handle_, FILE_MAP_READ, 0, 0, 0);
            if (addr_ == nullptr)
            {
                CloseHandle(handle_);
                handle_ = nullptr;

                LOGE(kErrorMemoryMap, path.c_str(), GetLastError());
            }
        }
    }
    CloseHandle(fh);
}

DataMapWin::DataMapWin(DataMapWin&& data) noexcept
    : len_(data.len_), off_(data.off_), addr_(data.addr_), handle_(data.handle_)
{
    data.len_ = 0;
    data.off_ = 0;
    data.addr_ = nullptr;
    data.handle_ = nullptr;
}

DataMapWin::~DataMapWin()
{
    if (handle_ == nullptr)
        return;

    UnmapViewOfFile(addr_);
    CloseHandle(handle_);
}
