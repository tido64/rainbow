// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef YGGDRASIL_CGENERATOR_H_
#define YGGDRASIL_CGENERATOR_H_

#include <unordered_map>

#include "ClangHelper.h"

namespace c
{
    auto generate(const std::vector<Class>& models)
    {
        std::ostringstream os;
        std::unordered_map<const void*, const char*> cursor_names;

        for (auto&& model : models)
        {
            os << "struct " << model.c_name() << ";\n";
            cursor_names[model.cursor().data] = model.c_name();
        }

        for (auto&& model : models)
        {
            os << '\n';
            for (auto&& ctor : model.constructors())
            {
                char fallback = 'p';
                os << model.c_name() << ' ' << ctor.entry_point_name() << '(';
                ctor.join_arguments(
                    std::ref(os),
                    [&fallback](std::ostream& os, const Argument& arg) mutable {
                        os << arg.spelling() << ' ';
                        if (arg.has_name())
                            os << arg.name();
                        else
                            os << fallback++;
                    });
                os << ");\n";
            }

            for (auto&& method : model.methods())
            {
                os << method.result().spelling() << ' ' << method.entry_point_name() << '(' << model.c_name() << '*';

                char fallback = 'p';
                for (auto&& arg : method.arguments())
                {
                    os << ", " << arg.spelling() << ' ';
                    if (arg.has_name())
                        os << arg.name();
                    else
                        os << fallback++;
                }

                os << ");\n";
            }
        }

        return os;
    }
}  // namespace c

#endif
