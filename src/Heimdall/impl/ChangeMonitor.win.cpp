// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Heimdall/ChangeMonitor.h"

#ifdef USE_HEIMDALL

#    include "Common/Logging.h"

using heimdall::ChangeMonitor;
using rainbow::czstring;

ChangeMonitor::ChangeMonitor(czstring directory)
    : monitoring_(false), callback_([](czstring) {})
{
    hDirectory_ =
        CreateFileA(directory,
                    FILE_LIST_DIRECTORY,
                    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                    nullptr,
                    OPEN_EXISTING,
                    FILE_FLAG_OVERLAPPED | FILE_FLAG_SEQUENTIAL_SCAN |
                        FILE_FLAG_BACKUP_SEMANTICS,
                    nullptr);
    if (hDirectory_ == INVALID_HANDLE_VALUE)
        return;

    LOGI("Monitoring '%s'", directory);
    monitoring_ = true;
    worker_ = std::async(std::launch::async, [this]() {
        char lpPath[MAX_PATH * 4];
        const DWORD nBufferLength = 8192;
        auto buffer = std::make_unique<DWORD[]>(nBufferLength);
        do {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            DWORD dwBytesReturned = 0;
            ReadDirectoryChangesW(
                hDirectory_,
                buffer.get(),
                nBufferLength,
                TRUE,
                FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_CREATION,
                &dwBytesReturned,
                nullptr,
                nullptr);
            if (dwBytesReturned == 0)
                continue;

            const uint8_t* lpBuffer = reinterpret_cast<uint8_t*>(buffer.get());
            const FILE_NOTIFY_INFORMATION* lpInfo = nullptr;
            do {
                if (!monitoring_)
                    return;

                lpInfo =
                    reinterpret_cast<const FILE_NOTIFY_INFORMATION*>(lpBuffer);
                if (lpInfo->Action != FILE_ACTION_MODIFIED)
                    continue;

                const int cchWideChar =
                    lpInfo->FileNameLength / sizeof(lpInfo->FileName[0]);
                const int length = WideCharToMultiByte(CP_UTF8,
                                                       0,
                                                       lpInfo->FileName,
                                                       cchWideChar,
                                                       nullptr,
                                                       0,
                                                       nullptr,
                                                       nullptr);
                if (length == 0)
                    continue;

                WideCharToMultiByte(CP_UTF8,
                                    0,
                                    lpInfo->FileName,
                                    cchWideChar,
                                    lpPath,
                                    length,
                                    nullptr,
                                    nullptr);
                lpPath[length] = '\0';
                on_modified(lpPath);

                lpBuffer += lpInfo->NextEntryOffset;
            } while (lpInfo->NextEntryOffset > 0);
        } while (monitoring_);
    });
}

ChangeMonitor::~ChangeMonitor()
{
    if (!monitoring_)
        return;

    monitoring_ = false;
    CancelIoEx(hDirectory_, nullptr);
    CloseHandle(hDirectory_);
}

#endif  // USE_HEIMDALL
