// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <iterator>
#include <tuple>
#include <vector>

#include "CGenerator.h"
#include "CSharpGenerator.h"
#include "LuaGenerator.h"

constexpr const char* const kSource[] = {
    "/Users/tido/Synced/Bifrost/Rainbow/src/Graphics/Animation.h",
    "/Users/tido/Synced/Bifrost/Rainbow/src/Graphics/SpriteBatch.h",
};

constexpr const char* const kWhitelist[] = {
    "rainbow::Animation",
    "rainbow::Sprite",
    "rainbow::SpriteBatch",
};

constexpr const char* const kCompilerFlags[] = {
    "-std=c++14",
    "-stdlib=libc++",
    "-isysroot",
    "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.12.sdk",
    "-isystem",
    "/usr/local/Cellar/llvm/4.0.1/include/c++/v1",
    "-I",
    "/usr/local/include",
    "-I",
    "/Users/tido/Synced/Bifrost/Rainbow/include",
    "-I",
    "/Users/tido/Synced/Bifrost/Rainbow/src",
    "-DUSE_SPRITE_FUNCTION_CHAINING=0",
};

auto main(int argc, char* argv[]) -> int
{
    std::ios::sync_with_stdio(false);

    std::vector<TranslationUnit> units;
    IR data{kWhitelist, array_size(kWhitelist)};
    for (auto&& src : kSource)
    {
        std::cout << "Parsing " << src << "...\n";

        units.emplace_back(src, kCompilerFlags, array_size(kCompilerFlags));
        auto& tu = units.back();
        if (!tu)
            continue;

        clang_visitChildren(tu.cursor(), visit_children, &data);
    }

/*
    for (auto&& model : data.models)
    {
        std::cout << model.name() << '\n';
        for (auto&& method : model.methods())
        {
            std::cout << "  + " << method.name() << " -> " << method.result().spelling() << '\n';
            for (auto&& arg : method.arguments())
            {
                std::cout << "    + " << arg.spelling() << ' ' << arg.name() << '\n';
            }
        }
    }
*/

    using ModelGenerator =
        std::add_pointer_t<std::ostringstream(const std::vector<Class>&)>;

    std::vector<std::tuple<ModelGenerator, const char*>> generators = {
        //std::make_tuple(c::generate, "Rainbow.h"),
        //std::make_tuple(cs::generate, "RainbowCore.g.cs"),
        std::make_tuple(lua::generate_body, "RainbowLua.g.cpp"),
    };

    for (auto&& generator : generators)
    {
        std::cout << "\nGenerating " << std::get<const char*>(generator) << "...\n\n";
        auto os = std::get<ModelGenerator>(generator)(data.models);
        std::cout << os.str();
    }

    return 0;
}
