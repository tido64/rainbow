// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef DOTNET_CORE_H_
#define DOTNET_CORE_H_

#include <string>

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
    auto native_dll_search_directories() -> std::string;
    auto trusted_platform_assemblies(const std::string& search_path)
        -> std::string;

    constexpr bool failed(int status) { return status < 0; }

    class CreateDelegate
    {
    public:
        CreateDelegate(CreateDelegateFn create_delegate,
                       void* host_handle,
                       unsigned int domain_id)
            : create_delegate_(create_delegate), host_handle_(host_handle),
              domain_id_(domain_id)
        {
        }

        template <typename F>
        bool operator()(const char* type,
                        const char* method_name,
                        F&& delegate,
                        Requirement requirement = Requirement::Required) const
        {
            return (*this)(type,
                           method_name,
                           reinterpret_cast<void**>(delegate),
                           requirement);
        }

    private:
        CreateDelegateFn create_delegate_;
        void* host_handle_;
        unsigned int domain_id_;

        bool operator()(const char* type,
                        const char* method_name,
                        void** delegate,
                        Requirement requirement) const;
    };
}

#undef CORECLR_HOSTING_API

#endif
