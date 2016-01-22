// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef LUA_FONT_H_
#define LUA_FONT_H_

#include "Graphics/FontAtlas.h"
#include "Lua/LuaBind.h"

NS_RAINBOW_LUA_BEGIN
{
    class Font : public Bind<Font>
    {
    public:
        Font(lua_State*);

        SharedPtr<FontAtlas> get() const;

    private:
        SharedPtr<FontAtlas> font_;
    };
} NS_RAINBOW_LUA_END

#endif
