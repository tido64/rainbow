// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "DotNet/SharedLibrary.h"

#include "Platform/Macros.h"
#ifdef RAINBOW_OS_WINDOWS
#   define RTLD_NOW    2
#   define RTLD_LOCAL  0
#else
#   include <dlfcn.h>
#endif

#include "Common/Logging.h"
#include "DotNet/Error.h"

using rainbow::SharedLibrary;
using rainbow::czstring;

namespace
{
#ifdef RAINBOW_OS_WINDOWS
    void dlclose(void* handle) { FreeLibrary(static_cast<HMODULE>(handle)); }

    auto dlopen(czstring filename, int) -> HMODULE
    {
        auto handle = LoadLibrary(filename);
        if (handle == nullptr)
            return nullptr;

        // Pin the module; coreclr.dll doesn't support being unloaded.
        HMODULE dummy;
        return GetModuleHandleEx(
                   GET_MODULE_HANDLE_EX_FLAG_PIN, filename, &dummy) == 0
                   ? nullptr
                   : handle;
    }

    auto dlsym(void* handle, czstring symbol)
    {
        return GetProcAddress(static_cast<HMODULE>(handle), symbol);
    }
#endif
}

SharedLibrary::SharedLibrary(std::string path) : path_(std::move(path))
{
    handle_ = dlopen(path_.c_str(), RTLD_NOW | RTLD_LOCAL);
    if (handle_ == nullptr)
    {
#ifdef RAINBOW_OS_WINDOWS
        auto dlerror = [path = path_.c_str()]
        {
            return error::format_message(
                GetLastError(), reinterpret_cast<DWORD_PTR>(path));
        };
#endif
        LOGF("%s", dlerror());
    }
}

SharedLibrary::~SharedLibrary()
{
    if (handle_ == nullptr)
        return;

    dlclose(handle_);
}

auto SharedLibrary::get_symbol(czstring symbol) const -> void*
{
    auto ptr = dlsym(handle_, symbol);
    if (ptr == nullptr)
        LOGF("Failed to load '%s' from '%s'", symbol, path_.c_str());
    return ptr;
}
