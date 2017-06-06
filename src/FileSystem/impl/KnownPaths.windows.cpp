// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "FileSystem/KnownPaths.h"

#include <memory>

#include <ShlObj.h>
#include <Shlwapi.h>

#include "Common/Algorithm.h"

#define TRY_NEXT()                                                             \
    do                                                                         \
    {                                                                          \
        get_known_folder_path(                                                 \
            std::ref(result), std::forward<Args>(fallback_fids)...);           \
        return;                                                                \
    } while (false)

namespace
{
    class ScopedCOM
    {
    public:
        ScopedCOM()
            : hresult_(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED))
        {
        }

        ~ScopedCOM()
        {
            if (*this)
                CoUninitialize();
        }

        explicit operator bool() const { return SUCCEEDED(hresult_); }

    private:
        HRESULT hresult_;
    };

    class ScopedUnknown
    {
    public:
        ScopedUnknown(IUnknown* unknown) : unknown_(unknown) {}
        ~ScopedUnknown() { unknown_->Release(); }

    private:
        IUnknown* unknown_;
    };

    auto to_u8string(LPWSTR str) -> std::string
    {
        const int size = WideCharToMultiByte(
            CP_UTF8, 0, str, -1, nullptr, 0, nullptr, nullptr);
        std::string result(size, '\0');
        WideCharToMultiByte(
            CP_UTF8, 0, str, -1, result.data(), size, nullptr, nullptr);
        result.pop_back();
        return result;
    }

    auto get_executable_path(DWORD buf_size = MAX_PATH) -> std::string
    {
        auto path = std::make_unique<WCHAR[]>(buf_size);
        GetModuleFileNameW(nullptr, path.get(), buf_size);

        const auto error = GetLastError();
        if (error == ERROR_INSUFFICIENT_BUFFER)
            return get_executable_path(buf_size * 2);

        return to_u8string(path.get());
    }

    void get_known_folder_path(std::string& result) { result = "."; }

    template <typename... Args>
    void get_known_folder_path(std::string& result,
                               REFKNOWNFOLDERID fid,
                               Args&&... fallback_fids)
    {
        LPWSTR path;
        if (FAILED(SHGetKnownFolderPath(fid, KF_FLAG_DEFAULT, nullptr, &path)))
            TRY_NEXT();

        constexpr WCHAR kLibraryExtension[] = L".library-ms";
        constexpr size_t kLibraryExtensionLength =
            rainbow::array_size(kLibraryExtension) - 1;

        const size_t path_len = wcslen(path);
        if (path_len > kLibraryExtensionLength &&
            CompareStringEx(LOCALE_NAME_INVARIANT,
                            0,
                            path + path_len - kLibraryExtensionLength,
                            -1,
                            kLibraryExtension,
                            -1,
                            nullptr,
                            nullptr,
                            0) == CSTR_EQUAL)
        {
            CoTaskMemFree(path);

            ScopedCOM com;
            if (!com)
                TRY_NEXT();

            IShellLibrary* library;
            if (FAILED(SHLoadLibraryFromKnownFolder(
                    fid, STGM_READ, IID_PPV_ARGS(&library))))
            {
                TRY_NEXT();
            }

            ScopedUnknown scoped_library(library);
            IShellItem* save_folder;
            if (FAILED(library->GetDefaultSaveFolder(
                    DSFT_DETECT, IID_PPV_ARGS(&save_folder))))
            {
                TRY_NEXT();
            }

            ScopedUnknown scoped_save_folder(save_folder);
            if (FAILED(save_folder->GetDisplayName(SIGDN_FILESYSPATH, &path)))
                TRY_NEXT();
        }

        result = to_u8string(path);
        CoTaskMemFree(path);
    }
}

namespace rainbow
{
    struct ISolemnlySwearThatIAmOnlyTesting;
}

namespace rainbow::filesystem
{
    auto get_executable_path() -> std::string
    {
        return ::get_executable_path();
    }

    auto get_save_directory() -> std::string
    {
        std::string save_directory;
        get_known_folder_path(std::ref(save_directory),
                              FOLDERID_SavedGames,
                              FOLDERID_DocumentsLibrary,
                              FOLDERID_Documents);
        return save_directory;
    }

    auto test_get_known_folder_path(REFKNOWNFOLDERID fid,
                                    const ISolemnlySwearThatIAmOnlyTesting&)
        -> std::string
    {
        std::string path;
        get_known_folder_path(std::ref(path), fid);
        return path;
    }
}  // namespace rainbow::filesystem
