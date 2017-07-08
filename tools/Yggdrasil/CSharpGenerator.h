// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef YGGDRASIL_CSHARPGENERATOR_H_
#define YGGDRASIL_CSHARPGENERATOR_H_

#include <iostream>
#include <sstream>

#include "ClangHelper.h"

namespace cs
{
    auto footer(std::ostream& os)
    {
        os << "    }\n}\n";
    }

    auto header(std::ostream& os)
    {
        banner(std::ref(os), "//") <<
            "namespace Rainbow.Interop\n"
            "{\n"
            "    using System;\n"
            "    using System.Runtime.InteropServices;\n"
            "\n"
            "    using Graphics;\n"
            "\n"
            "    internal static class Core\n"
            "    {\n"
            "        private const string RainbowCore = @\"rainbow.exe\";\n";
    }

    auto type_for(CXType type)
    {
        switch (type.kind)
        {
            case CXType_Unexposed:
                return "IntPtr";

            case CXType_Void:
                return "void";

            case CXType_Bool:
                return "bool";

            case CXType_UInt:
            case CXType_ULong:
                return "uint";

            case CXType_Int:
            case CXType_Long:
                return "int";

            case CXType_Float:
                return "float";

            case CXType_Double:
                return "double";

            case CXType_LValueReference:
            case CXType_Record:
                return "IntPtr";

            case CXType_Typedef:
                return type_for(clang_getCanonicalType(type));

            default:
                std::cout << "Unhandled type in type_for: " << Spelling(type) << " (" << type.kind << ")\n";
                return "<TODO>";
        }
    }

    auto generate(const std::vector<Class>& models)
    {
        std::ostringstream os;
        header(std::ref(os));

        for (auto&& model : models)
        {
            for (auto&& ctor : model.constructors())
            {
                os << "\n        [DllImport(RainbowCore, EntryPoint = @\"" << ctor.entry_point_name() << "\")]\n";
                os << "        public static extern IntPtr " << model.name() << "Create(";

                char fallback = 'p';
                ctor.join_arguments(
                    std::ref(os),
                    [&fallback](std::ostream& os, const Argument& arg) mutable {
                        os << type_for(arg.type()) << ' ';
                        if (arg.has_name())
                            os << arg.name();
                        else
                            os << fallback++;
                    });
                os << ");\n";
            }

            for (auto&& method : model.methods())
            {
                os << "\n        [DllImport(RainbowCore, EntryPoint = @\"" << method.entry_point_name() << "\")]\n";
                os << "        public static extern ";
                os << type_for(method.result().type()) << ' ';
                os << method.to_camelcase();
                os << "(IntPtr " << to_lower(model.name());

                char fallback = 'p';
                for (auto&& arg : method.arguments())
                {
                    os << ", " << type_for(arg.type()) << ' ';
                    if (arg.has_name())
                        os << arg.name();
                    else
                        os << fallback++;
                }

                os << ");\n";
            }
        }

        footer(std::ref(os));
        return os;
    }
}  // namespace cs

#endif
