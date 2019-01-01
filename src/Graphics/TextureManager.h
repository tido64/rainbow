// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_TEXTUREMANAGER_H_
#define GRAPHICS_TEXTUREMANAGER_H_

#include <cstdint>
#include <optional>
#include <string>
#include <tuple>

#ifdef __GNUC__
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wpedantic"
#    pragma GCC diagnostic ignored "-Woverflow"
#endif
#include <absl/container/flat_hash_map.h>
#ifdef __GNUC__
#    pragma GCC diagnostic pop
#endif

namespace rainbow
{
    struct Image;
}

namespace rainbow::vk
{
    class LogicalDevice;
}

namespace rainbow::graphics
{
    enum class Filter
    {
        Nearest,
        Linear,
        Cubic,
    };

    struct Texture
    {
        intptr_t data[4];
        uint32_t width = 0;
        uint32_t height = 0;
#ifdef USE_HEIMDALL
        uint32_t size = 0;
#endif
    };

    class TextureManager
    {
    public:
        TextureManager(const vk::LogicalDevice& device) : device_(device) {}
        ~TextureManager();

        auto device() const -> const vk::LogicalDevice& { return device_; }
        auto hash_function() const { return texture_map_.hash_function(); }

        auto get(std::string_view path,
                 float scale = 1.0f,
                 Filter mag_filter = Filter::Cubic,
                 Filter min_filter = Filter::Linear) -> const Texture&;

        auto get(std::string_view path,
                 const Image&,
                 Filter mag_filter = Filter::Cubic,
                 Filter min_filter = Filter::Linear) -> const Texture&;

        void release(std::string_view path);

        auto try_get(std::string_view path) const -> std::optional<Texture>;
        auto try_get(std::string_view path, size_t hash) const
            -> std::optional<Texture>;

        void update(std::string_view path, size_t hash, const Image&) const;

    private:
        using container_type = absl::flat_hash_map<std::string, Texture>;

        container_type texture_map_;
        const vk::LogicalDevice& device_;

        void load(const Image&,
                  Filter mag_filter,
                  Filter min_filter,
                  container_type::iterator);

#ifdef USE_HEIMDALL
    public:
        auto memory_usage() const
        {
            return std::make_tuple(mem_used_, mem_peak_);
        }

    private:
        size_t mem_used_ = 0;
        size_t mem_peak_ = 0;
#endif
    };
}  // namespace rainbow::graphics

namespace rainbow::vk
{
    class CommandBuffer;
    class Texture;

    auto to_texture(const graphics::Texture&) -> const Texture&;

    void update_descriptor(const CommandBuffer&,
                           const graphics::Texture&,
                           uint32_t binding = 1);
}  // namespace rainbow::vk

#endif
