// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <memory>

#include "Platform/Macros.h"
#ifdef RAINBOW_OS_WINDOWS
#   include <Windows.h>
#endif

#include "Common/String.h"
#include "Memory/TempBuffer.h"

namespace rainbow { namespace error
{
    // Error codes from coreclr/src/coreclr/hosts/corerun/logger.cpp
    inline auto to_string(unsigned long message_id) -> czstring
    {
        switch (message_id)
        {
            case 0x00000000:
                return "S_OK";
            case 0x00000001:
                return "S_FALSE";
            case 0x8000000B:
                return "E_BOUNDS";
            case 0x8000000C:
                return "E_CHANGED_STATE";
            case 0x80000013:
                return "RO_E_CLOSED";
            case 0x8000211D:
                return "COR_E_AMBIGUOUSMATCH";
            case 0x80004001:
                return "E_NOTIMPL";
            case 0x80004002:
                return "COR_E_INVALIDCAST";
            // case 0x80004002: return "E_NOINTERFACE";
            case 0x80004003:
                return "COR_E_NULLREFERENCE";
            // case 0x80004003: return "E_POINTER";
            case 0x80004004:
                return "E_ABORT";
            case 0x80004005:
                return "E_FAIL";
            case 0x8000FFFF:
                return "E_UNEXPECTED";
            case 0x8002000a:
                return "DISP_E_OVERFLOW";
            case 0x8002000e:
                return "COR_E_TARGETPARAMCOUNT";
            case 0x80020012:
                return "COR_E_DIVIDEBYZERO";
            case 0x80028ca0:
                return "TYPE_E_TYPEMISMATCH";
            case 0x80070005:
                return "COR_E_UNAUTHORIZEDACCESS";
            // case 0x80070005: return "E_ACCESSDENIED";
            case 0x80070006:
                return "E_HANDLE";
            case 0x8007000B:
                return "COR_E_BADIMAGEFORMAT";
            case 0x8007000E:
                return "COR_E_OUTOFMEMORY";
            // case 0x8007000E: return "E_OUTOFMEMORY";
            case 0x80070057:
                return "COR_E_ARGUMENT";
            // case 0x80070057: return "E_INVALIDARG";
            case 0x80070216:
                return "COR_E_ARITHMETIC";
            case 0x800703E9:
                return "COR_E_STACKOVERFLOW";
            case 0x80090020:
                return "NTE_FAIL";
            case 0x80131013:
                return "COR_E_TYPEUNLOADED";
            case 0x80131014:
                return "COR_E_APPDOMAINUNLOADED";
            case 0x80131015:
                return "COR_E_CANNOTUNLOADAPPDOMAIN";
            case 0x80131040:
                return "FUSION_E_REF_DEF_MISMATCH";
            case 0x80131047:
                return "FUSION_E_INVALID_NAME";
            case 0x80131416:
                return "CORSEC_E_POLICY_EXCEPTION";
            case 0x80131417:
                return "CORSEC_E_MIN_GRANT_FAIL";
            case 0x80131418:
                return "CORSEC_E_NO_EXEC_PERM";
            // case 0x80131418: return "CORSEC_E_XMLSYNTAX";
            case 0x80131430:
                return "CORSEC_E_CRYPTO";
            case 0x80131431:
                return "CORSEC_E_CRYPTO_UNEX_OPER";
            case 0x80131500:
                return "COR_E_EXCEPTION";
            case 0x80131501:
                return "COR_E_SYSTEM";
            case 0x80131502:
                return "COR_E_ARGUMENTOUTOFRANGE";
            case 0x80131503:
                return "COR_E_ARRAYTYPEMISMATCH";
            case 0x80131504:
                return "COR_E_CONTEXTMARSHAL";
            case 0x80131505:
                return "COR_E_TIMEOUT";
            case 0x80131506:
                return "COR_E_EXECUTIONENGINE";
            case 0x80131507:
                return "COR_E_FIELDACCESS";
            case 0x80131508:
                return "COR_E_INDEXOUTOFRANGE";
            case 0x80131509:
                return "COR_E_INVALIDOPERATION";
            case 0x8013150A:
                return "COR_E_SECURITY";
            case 0x8013150C:
                return "COR_E_SERIALIZATION";
            case 0x8013150D:
                return "COR_E_VERIFICATION";
            case 0x80131510:
                return "COR_E_METHODACCESS";
            case 0x80131511:
                return "COR_E_MISSINGFIELD";
            case 0x80131512:
                return "COR_E_MISSINGMEMBER";
            case 0x80131513:
                return "COR_E_MISSINGMETHOD";
            case 0x80131514:
                return "COR_E_MULTICASTNOTSUPPORTED";
            case 0x80131515:
                return "COR_E_NOTSUPPORTED";
            case 0x80131516:
                return "COR_E_OVERFLOW";
            case 0x80131517:
                return "COR_E_RANK";
            case 0x80131518:
                return "COR_E_SYNCHRONIZATIONLOCK";
            case 0x80131519:
                return "COR_E_THREADINTERRUPTED";
            case 0x8013151A:
                return "COR_E_MEMBERACCESS";
            case 0x80131520:
                return "COR_E_THREADSTATE";
            case 0x80131521:
                return "COR_E_THREADSTOP";
            case 0x80131522:
                return "COR_E_TYPELOAD";
            case 0x80131523:
                return "COR_E_ENTRYPOINTNOTFOUND";
            // case 0x80131523: return "COR_E_UNSUPPORTEDFORMAT";
            case 0x80131524:
                return "COR_E_DLLNOTFOUND";
            case 0x80131525:
                return "COR_E_THREADSTART";
            case 0x80131527:
                return "COR_E_INVALIDCOMOBJECT";
            case 0x80131528:
                return "COR_E_NOTFINITENUMBER";
            case 0x80131529:
                return "COR_E_DUPLICATEWAITOBJECT";
            case 0x8013152B:
                return "COR_E_SEMAPHOREFULL";
            case 0x8013152C:
                return "COR_E_WAITHANDLECANNOTBEOPENED";
            case 0x8013152D:
                return "COR_E_ABANDONEDMUTEX";
            case 0x80131530:
                return "COR_E_THREADABORTED";
            case 0x80131531:
                return "COR_E_INVALIDOLEVARIANTTYPE";
            case 0x80131532:
                return "COR_E_MISSINGMANIFESTRESOURCE";
            case 0x80131533:
                return "COR_E_SAFEARRAYTYPEMISMATCH";
            case 0x80131534:
                return "COR_E_TYPEINITIALIZATION";
            case 0x80131535:
                return "COR_E_COMEMULATE";
            // case 0x80131535: return "COR_E_MARSHALDIRECTIVE";
            case 0x80131536:
                return "COR_E_MISSINGSATELLITEASSEMBLY";
            case 0x80131537:
                return "COR_E_FORMAT";
            case 0x80131538:
                return "COR_E_SAFEARRAYRANKMISMATCH";
            case 0x80131539:
                return "COR_E_PLATFORMNOTSUPPORTED";
            case 0x8013153A:
                return "COR_E_INVALIDPROGRAM";
            case 0x8013153B:
                return "COR_E_OPERATIONCANCELED";
            case 0x8013153D:
                return "COR_E_INSUFFICIENTMEMORY";
            case 0x8013153E:
                return "COR_E_RUNTIMEWRAPPED";
            case 0x80131541:
                return "COR_E_DATAMISALIGNED";
            case 0x80131543:
                return "COR_E_TYPEACCESS";
            case 0x80131577:
                return "COR_E_KEYNOTFOUND";
            case 0x80131578:
                return "COR_E_INSUFFICIENTEXECUTIONSTACK";
            case 0x80131600:
                return "COR_E_APPLICATION";
            case 0x80131601:
                return "COR_E_INVALIDFILTERCRITERIA";
            case 0x80131602:
                return "COR_E_REFLECTIONTYPELOAD";
            case 0x80131603:
                return "COR_E_TARGET";
            case 0x80131604:
                return "COR_E_TARGETINVOCATION";
            case 0x80131605:
                return "COR_E_CUSTOMATTRIBUTEFORMAT";
            case 0x80131622:
                return "COR_E_OBJECTDISPOSED";
            case 0x80131623:
                return "COR_E_SAFEHANDLEMISSINGATTRIBUTE";
            case 0x80131640:
                return "COR_E_HOSTPROTECTION";
        }

        return nullptr;
    }

    template <typename... Args>
    auto format_message(unsigned long message_id, Args&&... args) -> czstring
    {
        zstring buffer = nullptr;

#ifdef RAINBOW_OS_WINDOWS
        constexpr uint32_t kBufferSize = 4096;
        constexpr bool kHasArguments = sizeof...(Args) > 0;
        constexpr DWORD kFormatMessageFlags =
            FORMAT_MESSAGE_FROM_SYSTEM |
            (kHasArguments ? FORMAT_MESSAGE_ARGUMENT_ARRAY : 0);

        buffer = get_temp_buffer<char>(kBufferSize);
        DWORD_PTR arguments[]{args..., 0};
        FormatMessageA(  //
            kFormatMessageFlags,
            nullptr,
            message_id,
            0,
            buffer,
            kBufferSize,
            kHasArguments ? reinterpret_cast<va_list*>(arguments) : nullptr);
#else
        NOT_USED(message_id);
        [](Args&&...) {}(std::forward<Args>(args)...);
#endif

        return (buffer == nullptr || *buffer == '\0') ? to_string(message_id)
                                                      : buffer;
    }
}}  // namespace rainbow::error
