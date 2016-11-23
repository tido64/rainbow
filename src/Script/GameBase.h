// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef SCRIPT_GAMEBASE_H_
#define SCRIPT_GAMEBASE_H_

#include "Director.h"
#include "Graphics/Animation.h"
#include "Graphics/Label.h"
#include "Graphics/SpriteBatch.h"
#include "Script/Prose.h"

namespace rainbow
{
    using animation_t = std::shared_ptr<Animation>;
    using font_t = SharedPtr<FontAtlas>;
    using label_t = std::shared_ptr<Label>;
    using prose_t = std::shared_ptr<Prose>;
    using spritebatch_t = std::shared_ptr<SpriteBatch>;
    using texture_t = SharedPtr<TextureAtlas>;

    template <typename... Args>
    auto animation(Args&&... args)
    {
        return std::make_shared<Animation>(std::forward<Args>(args)...);
    }

    template <typename... Args>
    auto font(Args&&... args)
    {
        auto font =
            rainbow::make_shared<FontAtlas>(std::forward<Args>(args)...);
        R_ASSERT(font->is_valid(), "rainbow::font: Failed to create font");
        return font;
    }

    inline auto label(const char* string = nullptr)
    {
        auto label = std::make_shared<Label>();
        if (string != nullptr)
            label->set_text(string);
        return label;
    }

    template <typename... Args>
    auto spritebatch(Args&&... args)
    {
        return std::make_shared<SpriteBatch>(std::forward<Args>(args)...);
    }

    template <typename... Args>
    auto texture(Args&&... args)
    {
        auto texture =
            rainbow::make_shared<TextureAtlas>(std::forward<Args>(args)...);
        R_ASSERT(
            texture->is_valid(), "rainbow::texture: Failed to create texture");
        return texture;
    }

    namespace prose
    {
        inline auto from_lua(const char* path)
        {
            return std::shared_ptr<Prose>(Prose::from_lua(path));
        }
    }

    class GameBase
    {
    public:
        static auto create(Director& director) -> std::unique_ptr<GameBase>;

        virtual ~GameBase() {}

        auto input() -> Input& { return director_.input(); }
        auto scenegraph() -> SceneNode& { return director_.scenegraph(); }

        void terminate() { director_.terminate(); }
        void terminate(const char* error) { director_.terminate(error); }

        void init(const Vec2i& screen_size) { init_impl(screen_size); }
        void update(uint64_t dt) { update_impl(dt); }
        void on_memory_warning() { on_memory_warning_impl(); }

    protected:
        GameBase(Director& director) : director_(director) {}

    private:
        Director& director_;

        virtual void init_impl(const Vec2i&) {}
        virtual void update_impl(uint64_t) {}
        virtual void on_memory_warning_impl() {}
    };
}

#endif
