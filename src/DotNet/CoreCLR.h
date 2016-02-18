// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef DOTNET_CORE_H_
#define DOTNET_CORE_H_

#include "Common/String.h"
#include "Platform/Macros.h"

#ifdef RAINBOW_OS_WINDOWS
#   define ASSEMBLY_SEPARATOR ';'
#else
#   define ASSEMBLY_SEPARATOR ':'
#endif

#if !HAS_FILESYSTEM
#   define BOOST_FILESYSTEM_NO_DEPRECATED
#   ifdef __clang__
#       pragma clang diagnostic push
#       pragma clang diagnostic ignored "-Wold-style-cast"
#   endif
#   include <boost/filesystem.hpp>
#   ifdef __clang__
#       pragma clang diagnostic pop
#   endif
namespace std
{
    namespace filesystem = boost::filesystem;
}
#else
#   include <experimental/filesystem>

namespace std
{
    namespace filesystem = std::experimental::filesystem;
}
#endif

// API defined with the following macro is licensed to the .NET Foundation under
// one or more agreements. The .NET Foundation licenses this API to you under
// the MIT license. See coreclr/src/coreclr/hosts/inc/coreclrhost.h.
#define CORECLR_HOSTING_API(function, name, ...)                               \
    using name##Fn = int (*)(__VA_ARGS__);                                     \
    constexpr char k##name##Function[] = function

namespace coreclr
{
    enum class Requirement
    {
        Optional,
        Required,
    };

    /// <summary>
    ///   Creates a native callable delegate for a managed method.
    /// </summary>
    CORECLR_HOSTING_API("coreclr_create_delegate",
                        CreateDelegate,
                        void* host_handle,
                        unsigned int domain_id,
                        const char* entry_point_assembly_name,
                        const char* entry_point_type_name,
                        const char* entry_point_method_name,
                        void** delegate);

    /// <summary>Executes a managed assembly with given arguments.</summary>
    CORECLR_HOSTING_API("coreclr_execute_assembly",
                        ExecuteAssembly,
                        void* host_handle,
                        unsigned int domain_id,
                        int argc,
                        const char** argv,
                        const char* managed_assembly_path,
                        unsigned int* exit_code);

    /// <summary>
    ///   Initializes the CoreCLR. Creates and starts CoreCLR host, and creates
    ///   an app domain.
    /// </summary>
    CORECLR_HOSTING_API("coreclr_initialize",
                        Initialize,
                        const char* exe_path,
                        const char* app_domain_friendly_name,
                        int property_count,
                        const char** property_keys,
                        const char** property_values,
                        void** host_handle,
                        unsigned int* domain_id);

    /// <summary>
    ///   Shutdowns CoreCLR. Unloads the app domain and stops the CoreCLR host.
    /// </summary>
    CORECLR_HOSTING_API("coreclr_shutdown",
                        Shutdown,
                        void* host_handle,
                        unsigned int domain_id);

    auto core_library_path() -> std::string;

    constexpr bool failed(int status) { return status < 0; }

    template <typename... Paths>
    auto find_assemblies(Paths&&... paths) -> std::string
    {
        std::string ni_assemblies;
        std::string il_assemblies;
        auto find = [&ni_assemblies,
                     &il_assemblies](const std::filesystem::path& path) {
            for (auto&& entry : std::filesystem::directory_iterator{path})
            {
                if (!std::filesystem::is_regular_file(entry.status()))
                    continue;

                const auto& p = entry.path();
                if (p.extension() == ".dll")
                {
                    if (rainbow::ends_with(p.stem().string(), ".ni"))
                        ni_assemblies += p.string() + ASSEMBLY_SEPARATOR;
                    else
                        il_assemblies += p.string() + ASSEMBLY_SEPARATOR;
                }
            }
        };

        const int fold[] = {(find(std::forward<Paths>(paths)), 0)...};
        NOT_USED(fold);

        ni_assemblies += il_assemblies;
        if (!ni_assemblies.empty())
            ni_assemblies.pop_back();

        return ni_assemblies;
    }

    auto native_dll_search_directories() -> std::string;

    class CreateDelegateHelper
    {
    public:
        CreateDelegateHelper(CreateDelegateFn create_delegate,
                             void* host_handle,
                             unsigned int domain_id)
            : create_delegate_(create_delegate), host_handle_(host_handle),
              domain_id_(domain_id)
        {
        }

        template <typename F>
        bool operator()(rainbow::czstring assembly,
                        rainbow::czstring type,
                        rainbow::czstring method_name,
                        F&& delegate,
                        Requirement requirement = Requirement::Required) const
        {
            return (*this)(assembly,
                           type,
                           method_name,
                           reinterpret_cast<void**>(delegate),
                           requirement);
        }

    private:
        CreateDelegateFn create_delegate_;
        void* host_handle_;
        unsigned int domain_id_;

        bool operator()(rainbow::czstring assembly,
                        rainbow::czstring type,
                        rainbow::czstring method_name,
                        void** delegate,
                        Requirement requirement) const;
    };
}

#undef CORECLR_HOSTING_API
#undef ASSEMBLY_SEPARATOR

#endif
