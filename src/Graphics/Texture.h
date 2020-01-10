// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_TEXTURE_H_
#define GRAPHICS_TEXTURE_H_

#include <array>
#include <optional>
#include <string>

#include "Common/NonCopyable.h"
#include "Common/Passkey.h"
#include "Memory/ArrayMap.h"

namespace rainbow
{
    class Data;
    struct Image;
    struct ISolemnlySwearThatIAmOnlyTesting;
}  // namespace rainbow

namespace rainbow::graphics
{
    struct Context;
    struct ITextureAllocator;
    class Texture;

    using TextureHandle = std::array<intptr_t, 4>;

    enum class Filter
    {
        Nearest,
        Linear,
        Cubic,
    };

    struct TextureData
    {
        TextureHandle data{};
        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t use_count = 0;
#ifdef USE_HEIMDALL
        uint32_t size = 0;
#endif
    };

    class TextureProvider : private NonCopyable<TextureProvider>
    {
    public:
        explicit TextureProvider(ITextureAllocator&);
        ~TextureProvider();

        [[nodiscard]]
        auto get(std::string_view path,
                 float scale = 1.0F,
                 Filter mag_filter = Filter::Cubic,
                 Filter min_filter = Filter::Linear) -> Texture;

        [[nodiscard]]
        auto get(std::string_view path,
                 const Data&,
                 float scale = 1.0F,
                 Filter mag_filter = Filter::Cubic,
                 Filter min_filter = Filter::Linear) -> Texture;

        [[nodiscard]]
        auto get(std::string_view path,
                 const Image&,
                 Filter mag_filter = Filter::Cubic,
                 Filter min_filter = Filter::Linear) -> Texture;

        [[nodiscard]]
        auto raw_get(const Texture&) const -> TextureData;

        void release(const Texture&);

        [[nodiscard]]
        auto try_get(const Texture&) -> std::optional<TextureData>;

        void update(const Texture&,
                    const Image&,
                    Filter mag_filter = Filter::Cubic,
                    Filter min_filter = Filter::Linear);

    private:
        using TextureMap = ArrayMap<std::string, TextureData>;

        TextureMap texture_map_;
        ITextureAllocator& allocator_;

        template <typename T>
        auto get(std::string_view path,
                 T,
                 float scale,
                 Filter mag_filter,
                 Filter min_filter) -> Texture;

        void load(TextureMap::iterator i,
                  const Image&,
                  Filter mag_filter,
                  Filter min_filter);

#ifdef USE_HEIMDALL
    public:
        [[nodiscard]]
        auto memory_usage() const
        {
            return std::make_tuple(mem_used_, mem_peak_);
        }

    private:
        size_t mem_used_ = 0;
        size_t mem_peak_ = 0;

        void record_usage(size_t image_size)
        {
            mem_used_ += image_size;
            if (mem_used_ > mem_peak_)
                mem_peak_ = mem_used_;
        }
#endif
    };

    class Texture
    {
    public:
        Texture() = default;
        Texture(const Texture&) = delete;
        Texture(Texture&& texture) noexcept : key_(std::move(texture.key_)) {}
        Texture(std::string_view key, Passkey<TextureProvider>) : key_(key) {}
        ~Texture();

        [[nodiscard]] auto key() const { return std::string_view{key_}; }

        auto operator=(const Texture&) -> Texture&;
        auto operator=(Texture&&) noexcept -> Texture&;

        explicit operator bool() const { return !key_.empty(); }

#ifdef RAINBOW_TEST
        Texture(std::string_view key, const ISolemnlySwearThatIAmOnlyTesting&)
            : key_(key)
        {
        }
#endif  // RAINBOW_TEST

    private:
        static TextureProvider* s_texture_provider;

        std::string key_;

        friend TextureProvider;
    };

    struct ITextureAllocator
    {
        virtual void construct(TextureHandle&,
                               const Image&,
                               Filter mag_filter,
                               Filter min_filter) = 0;

        virtual void destroy(TextureHandle&) = 0;

        [[maybe_unused, nodiscard]]
        virtual auto max_size() const noexcept -> size_t = 0;

        virtual void update(const TextureHandle&,
                            const Image&,
                            Filter mag_filter,
                            Filter min_filter) = 0;
    };

    void bind(const Context&, const Texture&, uint32_t unit = 0);
}  // namespace rainbow::graphics

#endif
