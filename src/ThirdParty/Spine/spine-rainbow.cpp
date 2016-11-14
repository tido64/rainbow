// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "ThirdParty/Spine/spine-rainbow.h"

#include <algorithm>

#include <spine/SkeletonJson.h>
#include <spine/extension.h>

#include "Common/DataMap.h"
#include "FileSystem/FileSystem.h"
#include "Graphics/Renderer.h"
#include "Graphics/SpriteVertex.h"
#include "Graphics/TextureAtlas.h"

namespace
{
#ifndef NDEBUG
    const char kErrorMultipleTexturesUnsupported[] =  // TODO: Implement.
        "Skeletons spanning multiple textures are not yet supported";
#endif

    int g_buffer_size = 0;
    std::unique_ptr<float[]> g_buffer;

    void compute_world_vertices(spMeshAttachment* self,
                                spSlot* slot,
                                float* vertices)
    {
        spMeshAttachment_computeWorldVertices(self, slot, vertices);
    }

    void compute_world_vertices(spSkinnedMeshAttachment* self,
                                spSlot* slot,
                                float* vertices)
    {
        spSkinnedMeshAttachment_computeWorldVertices(self, slot, vertices);
    }

    template <typename F>
    auto for_each(const spSkeleton* skeleton, F&& f)
    {
        return std::for_each(skeleton->drawOrder,
                             skeleton->drawOrder + skeleton->slotsCount,
                             std::forward<F>(f));
    }

    template <typename T>
    auto get_texture(const T* attachment)
    {
        return static_cast<TextureAtlas*>(
            static_cast<spAtlasRegion*>(attachment->rendererObject)
                ->page->rendererObject);
    }

    auto get_vertex_count(const spSkeleton* skeleton, TextureAtlas** atlas)
        -> size_t
    {
        size_t num_vertices = 0;
        TextureAtlas* texture = nullptr;
        for_each(skeleton, [&num_vertices, &texture](const spSlot* slot) {
            if (slot->attachment == nullptr)
                return;
            switch (slot->attachment->type)
            {
                case SP_ATTACHMENT_REGION:
                    num_vertices += 6;
                    texture = get_texture(
                        reinterpret_cast<spRegionAttachment*>(
                            slot->attachment));
                    break;
                case SP_ATTACHMENT_BOUNDING_BOX:
                    break;
                case SP_ATTACHMENT_MESH: {
                    spMeshAttachment* mesh =
                        reinterpret_cast<spMeshAttachment*>(slot->attachment);
                    num_vertices += mesh->trianglesCount;
                    texture = get_texture(mesh);
                    break;
                }
                case SP_ATTACHMENT_SKINNED_MESH: {
                    spSkinnedMeshAttachment* mesh =
                        reinterpret_cast<spSkinnedMeshAttachment*>(
                            slot->attachment);
                    num_vertices += mesh->trianglesCount;
                    texture = get_texture(mesh);
                    break;
                }
            }
        });
        if (atlas != nullptr)
            *atlas = texture;
        return num_vertices;
    }

#if USE_LUA_SCRIPT
    void on_animation_state_event(spAnimationState* state,
                                  int track,
                                  spEventType type,
                                  spEvent* event,
                                  int loop_count)
    {
        using spine::lua::Skeleton;

        Skeleton* sk = static_cast<Skeleton*>(state->rendererObject);
        sk->listener().get();
        if (lua_isnil(sk->state(), -1))
        {
            lua_pop(sk->state(), 1);
            sk->get()->set_listener(nullptr, nullptr);
            return;
        }

        switch (type)
        {
            case SP_ANIMATION_START:
                lua_getfield(sk->state(), -1, "on_animation_start");
                break;
            case SP_ANIMATION_END:
                lua_getfield(sk->state(), -1, "on_animation_end");
                break;
            case SP_ANIMATION_COMPLETE:
                lua_getfield(sk->state(), -1, "on_animation_complete");
                break;
            case SP_ANIMATION_EVENT:
                lua_getfield(sk->state(), -1, "on_animation_event");
                break;
        }
        if (!lua_isfunction(sk->state(), -1))
        {
            lua_pop(sk->state(), 2);
            return;
        }

        lua_insert(sk->state(), -2);
        lua_pushinteger(sk->state(), track);
        int nargs = 3;
        if (event != nullptr)
        {
            R_ASSERT(type == SP_ANIMATION_EVENT,
                     "Should only occur on animation events");

            lua_pushstring(
                sk->state(),
                (event->data == nullptr ? nullptr : event->data->name));
            ++nargs;
        }
        lua_pushinteger(sk->state(), loop_count);
        rainbow::lua::call(
            sk->state(), nargs, 0, 0,
            "An error occurred while handling an animation state event");
    }
#endif  // USE_LUA_SCRIPT

    template <typename T>
    auto update_mesh(SpriteVertex* vertices,
                     const spSkeleton* skeleton,
                     T* mesh,
                     spSlot* slot) -> int
    {
        if (mesh->trianglesCount > g_buffer_size)
        {
            g_buffer_size = mesh->trianglesCount * 2;
            g_buffer = std::make_unique<float[]>(g_buffer_size);
        }

        float* coordinates = g_buffer.get();
        compute_world_vertices(mesh, slot, coordinates);

        const char r = skeleton->r * slot->r * 0xff;
        const char g = skeleton->g * slot->g * 0xff;
        const char b = skeleton->b * slot->b * 0xff;
        const char a = skeleton->a * slot->a * 0xff;

        for (int i = 0; i < mesh->trianglesCount; ++i)
        {
            vertices[i].color.r = r;
            vertices[i].color.g = g;
            vertices[i].color.b = b;
            vertices[i].color.a = a;

            const int index = mesh->triangles[i] << 1;
            vertices[i].texcoord.x = mesh->uvs[index];
            vertices[i].texcoord.y = mesh->uvs[index + 1];
            vertices[i].position.x = coordinates[index];
            vertices[i].position.y = coordinates[index + 1];
        }
        return mesh->trianglesCount;
    }
}

extern "C"
{
    void _spAtlasPage_createTexture(spAtlasPage* self, const char* path)
    {
        auto texture =
            std::make_unique<TextureAtlas>(rainbow::filesystem::relative(path));
        if (!texture->is_valid())
            return;

        self->width = texture->width();
        self->height = texture->height();
        self->rendererObject = texture.release();
    }

    void _spAtlasPage_disposeTexture(spAtlasPage* self)
    {
        delete static_cast<TextureAtlas*>(self->rendererObject);
    }

    auto _spUtil_readFile(const char* path, int* length) -> char*
    {
        const DataMap data{rainbow::filesystem::relative(path)};
        *length = static_cast<int>(data.size());
        auto blob = new char[data.size()];
        memcpy(blob, data.data(), data.size());
        return blob;
    }
}  // extern "C"

auto Skeleton::from_json(const char* path, float scale) -> Skeleton*
{
    // Copy the path and replace ".json" with ".atlas"
    const size_t length = strlen(path);
    const char* ext = path + length - 5;
    if (*ext != '.')
    {
        ext = strrchr(path, '.');
        if (ext == nullptr)
            ext = path + length;
    }
    const std::string atlas_path = std::string{path, ext} + ".atlas";
    spAtlas* atlas = spAtlas_createFromFile(atlas_path.c_str(), nullptr);
    spSkeletonJson* json = spSkeletonJson_create(atlas);
    json->scale = scale;
    spSkeletonData* data = spSkeletonJson_readSkeletonDataFile(json, path);
    spSkeletonJson_dispose(json);
    if (data == nullptr)
    {
        LOGE("Spine: %s", json->error);
        return nullptr;
    }
    return new Skeleton(data, atlas);
}

Skeleton::Skeleton(spSkeletonData* data, spAtlas* atlas)
    : skeleton_(nullptr), state_(nullptr), time_scale_(1.0f), num_vertices_(0),
      texture_(nullptr), max_vertices_(0), animation_data_(nullptr),
      atlas_(atlas), data_(data)
{
    skeleton_ = spSkeleton_create(data);
    spSkeleton_setToSetupPose(skeleton_);
    animation_data_ = spAnimationStateData_create(data);
    state_ = spAnimationState_create(animation_data_);

    array_.reconfigure([this] { vertex_buffer_.bind(); });
}

Skeleton::~Skeleton()
{
    spAnimationStateData_dispose(animation_data_);
    spAnimationState_dispose(state_);
    spSkeleton_dispose(skeleton_);
    spSkeletonData_dispose(data_);
    spAtlas_dispose(atlas_);
}

void Skeleton::set_flip(bool x, bool y)
{
    skeleton_->flipX = x;
    skeleton_->flipY = y;
}

void Skeleton::set_listener(spAnimationStateListener listener, void* self)
{
    state_->listener = listener;
    state_->rendererObject = self;
}

void Skeleton::set_position(const Vec2f& p)
{
    skeleton_->x = p.x;
    skeleton_->y = p.y;
}

void Skeleton::add_animation(int track,
                             const char* animation,
                             bool loop,
                             float delay)
{
    spAnimationState_addAnimationByName(state_, track, animation, loop, delay);
}

void Skeleton::bind_textures() const
{
    texture_->bind();
}

void Skeleton::clear_track(int track)
{
    spAnimationState_clearTrack(state_, track);
}

void Skeleton::clear_tracks()
{
    spAnimationState_clearTracks(state_);
}

auto Skeleton::get_current_animation(int track) -> const char*
{
    spTrackEntry* entry = spAnimationState_getCurrent(state_, track);
    return entry == nullptr ? nullptr : entry->animation->name;
}

auto Skeleton::get_skin() -> const char*
{
    return skeleton_->skin->name;
}

void Skeleton::set_animation(int track, const char* animation, bool loop)
{
    spAnimationState_setAnimationByName(state_, track, animation, loop);
}

void Skeleton::set_animation_mix(const char* from,
                                 const char* to,
                                 float duration)
{
    spAnimationStateData_setMixByName(animation_data_, from, to, duration);
}

void Skeleton::set_attachment(const char* slot, const char* attachment)
{
    spSkeleton_setAttachment(skeleton_, slot, attachment);
}

void Skeleton::set_skin(const char* skin)
{
    spSkeleton_setSkinByName(skeleton_, skin);
    spSkeleton_setToSetupPose(skeleton_);
}

void Skeleton::draw()
{
    rainbow::graphics::draw_arrays(*this, 0, num_vertices_);
}

void Skeleton::update(uint64_t dt)
{
    const float delta = dt / 1000.0f;
    spSkeleton_update(skeleton_, delta);
    spAnimationState_update(state_, delta * time_scale_);
    spAnimationState_apply(state_, skeleton_);
    spSkeleton_updateWorldTransform(skeleton_);

    // This solution is not ideal. We iterate over the bones twice: First to
    // determine number of vertices, second to update the vertex buffer.
    num_vertices_ = get_vertex_count(skeleton_, &texture_);
    if (num_vertices_ > max_vertices_)
    {
        max_vertices_ = num_vertices_;
        vertices_ = std::make_unique<SpriteVertex[]>(max_vertices_);
    }

    size_t i = 0;
    for_each(skeleton_, [this, &i](spSlot* slot) {
        if (slot->attachment == nullptr)
            return;

        switch (slot->attachment->type)
        {
            case SP_ATTACHMENT_REGION: {
                float vertices[8];
                spRegionAttachment* region =
                    reinterpret_cast<spRegionAttachment*>(slot->attachment);
                R_ASSERT(texture_ == get_texture(region),
                         kErrorMultipleTexturesUnsupported);
                spRegionAttachment_computeWorldVertices(
                    region, slot->bone, vertices);

                const char r = skeleton_->r * slot->r * 0xff;
                const char g = skeleton_->g * slot->g * 0xff;
                const char b = skeleton_->b * slot->b * 0xff;
                const char a = skeleton_->a * slot->a * 0xff;

                vertices_[i].color.r = r;
                vertices_[i].color.g = g;
                vertices_[i].color.b = b;
                vertices_[i].color.a = a;
                vertices_[i].texcoord.x = region->uvs[SP_VERTEX_X1];
                vertices_[i].texcoord.y = region->uvs[SP_VERTEX_Y1];
                vertices_[i].position.x = vertices[SP_VERTEX_X1];
                vertices_[i].position.y = vertices[SP_VERTEX_Y1];

                vertices_[++i].color.r = r;
                vertices_[i].color.g = g;
                vertices_[i].color.b = b;
                vertices_[i].color.a = a;
                vertices_[i].texcoord.x = region->uvs[SP_VERTEX_X2];
                vertices_[i].texcoord.y = region->uvs[SP_VERTEX_Y2];
                vertices_[i].position.x = vertices[SP_VERTEX_X2];
                vertices_[i].position.y = vertices[SP_VERTEX_Y2];

                vertices_[++i].color.r = r;
                vertices_[i].color.g = g;
                vertices_[i].color.b = b;
                vertices_[i].color.a = a;
                vertices_[i].texcoord.x = region->uvs[SP_VERTEX_X3];
                vertices_[i].texcoord.y = region->uvs[SP_VERTEX_Y3];
                vertices_[i].position.x = vertices[SP_VERTEX_X3];
                vertices_[i].position.y = vertices[SP_VERTEX_Y3];

                ++i;
                vertices_[i] = vertices_[i - 1];

                vertices_[++i].color.r = r;
                vertices_[i].color.g = g;
                vertices_[i].color.b = b;
                vertices_[i].color.a = a;
                vertices_[i].texcoord.x = region->uvs[SP_VERTEX_X4];
                vertices_[i].texcoord.y = region->uvs[SP_VERTEX_Y4];
                vertices_[i].position.x = vertices[SP_VERTEX_X4];
                vertices_[i].position.y = vertices[SP_VERTEX_Y4];

                ++i;
                vertices_[i] = vertices_[i - 5];

                ++i;
                break;
            }
            case SP_ATTACHMENT_BOUNDING_BOX:
                break;
            case SP_ATTACHMENT_MESH: {
                spMeshAttachment* mesh =
                    reinterpret_cast<spMeshAttachment*>(slot->attachment);
                R_ASSERT(texture_ == get_texture(mesh),
                         kErrorMultipleTexturesUnsupported);
                i += update_mesh(&vertices_[i], skeleton_, mesh, slot);
                break;
            }
            case SP_ATTACHMENT_SKINNED_MESH: {
                spSkinnedMeshAttachment* mesh =
                    reinterpret_cast<spSkinnedMeshAttachment*>(
                        slot->attachment);
                R_ASSERT(texture_ == get_texture(mesh),
                         kErrorMultipleTexturesUnsupported);
                i += update_mesh(&vertices_[i], skeleton_, mesh, slot);
                break;
            }
        }
        if (slot->data->blendMode != SP_BLEND_MODE_NORMAL)  // TODO: Implement.
            LOGE("Non-normal blend mode not yet implemented");
    });

    vertex_buffer_.upload(vertices_.get(), i * sizeof(SpriteVertex));
}

#if USE_LUA_SCRIPT
namespace spine { namespace lua
{
    constexpr bool Skeleton::is_constructible;

    const char Skeleton::class_name[] = "skeleton";

    const luaL_Reg Skeleton::functions[]{
        {"add_animation",          &Skeleton::add_animation},
        {"clear_track",            &Skeleton::clear_track},
        {"clear_tracks",           &Skeleton::clear_tracks},
        {"get_current_animation",  &Skeleton::get_current_animation},
        {"get_skin",               &Skeleton::get_skin},
        {"set_animation",          &Skeleton::set_animation},
        {"set_animation_mix",      &Skeleton::set_animation_mix},
        {"set_attachment",         &Skeleton::set_attachment},
        {"set_flip",               &Skeleton::set_flip},
        {"set_listener",           &Skeleton::set_listener},
        {"set_position",           &Skeleton::set_position},
        {"set_skin",               &Skeleton::set_skin},
        {"set_time_scale",         &Skeleton::set_time_scale},
        {nullptr,                  nullptr}};

    Skeleton::Skeleton(lua_State* L) : state_(L)
    {
        rainbow::lua::checkargs<char*, rainbow::lua::nil_or<lua_Number>>(L);

        skeleton_.reset(::Skeleton::from_json(
            lua_tostring(L, 1), rainbow::lua::optnumber(L, 2, 1.0f)));
    }

    auto Skeleton::add_animation(lua_State* L) -> int
    {
        rainbow::lua::
            checkargs<Skeleton, lua_Number, char*, bool, lua_Number>(L);

        Skeleton* self = Bind::self(L);
        if (self == nullptr)
            return 0;

        self->skeleton_->add_animation(
            lua_tointeger(L, 2),
            lua_tostring(L, 3),
            rainbow::lua::toboolean(L, 4),
            lua_tonumber(L, 5));
        return 0;
    }

    auto Skeleton::clear_track(lua_State* L) -> int
    {
        return set1i(L, [](::Skeleton* skeleton, int track) {
            skeleton->clear_track(track);
        });
    }

    auto Skeleton::clear_tracks(lua_State* L) -> int
    {
        Skeleton* self = Bind::self(L);
        if (self == nullptr)
            return 0;

        self->skeleton_->clear_tracks();
        return 0;
    }

    auto Skeleton::get_current_animation(lua_State* L) -> int
    {
        rainbow::lua::checkargs<Skeleton, lua_Number>(L);

        Skeleton* self = Bind::self(L);
        if (self == nullptr)
            return 0;

        const char* name =
            self->skeleton_->get_current_animation(lua_tointeger(L, 2));
        if (name == nullptr)
            return 0;

        lua_pushstring(L, name);
        return 1;
    }

    auto Skeleton::get_skin(lua_State* L) -> int
    {
        Skeleton* self = Bind::self(L);
        if (self == nullptr)
            return 0;

        lua_pushstring(L, self->skeleton_->get_skin());
        return 1;
    }

    auto Skeleton::set_animation(lua_State* L) -> int
    {
        rainbow::lua::checkargs<Skeleton, lua_Number, char*, bool>(L);

        Skeleton* self = Bind::self(L);
        if (self == nullptr)
            return 0;

        self->skeleton_->set_animation(
            lua_tointeger(L, 2),
            lua_tostring(L, 3),
            rainbow::lua::toboolean(L, 4));
        return 0;
    }

    auto Skeleton::set_animation_mix(lua_State* L) -> int
    {
        rainbow::lua::checkargs<Skeleton, char*, char*, lua_Number>(L);

        Skeleton* self = Bind::self(L);
        if (self == nullptr)
            return 0;

        self->skeleton_->set_animation_mix(
            lua_tostring(L, 2),
            lua_tostring(L, 3),
            lua_tonumber(L, 4));
        return 0;
    }

    auto Skeleton::set_attachment(lua_State* L) -> int
    {
        rainbow::lua::
            checkargs<Skeleton, char*, rainbow::lua::nil_or<char*>>(L);

        Skeleton* self = Bind::self(L);
        if (self == nullptr)
            return 0;

        self->skeleton_->set_attachment(lua_tostring(L, 2),
                                        lua_tostring(L, 3));
        return 0;
    }

    auto Skeleton::set_flip(lua_State* L) -> int
    {
        rainbow::lua::checkargs<Skeleton, bool, bool>(L);

        Skeleton* self = Bind::self(L);
        if (self == nullptr)
            return 0;

        self->skeleton_->set_flip(
            rainbow::lua::toboolean(L, 2), rainbow::lua::toboolean(L, 3));
        return 0;
    }

    auto Skeleton::set_listener(lua_State* L) -> int
    {
        rainbow::lua::checkargs<Skeleton, rainbow::lua::nil_or<void*>>(L);

        Skeleton* self = Bind::self(L);
        if (self == nullptr)
            return 0;

        if (!lua_istable(L, 2))
        {
            self->listener_.reset();
            self->skeleton_->set_listener(nullptr, nullptr);
        }
        else
        {
            lua_settop(L, 2);
            self->listener_.reset(L);
            self->skeleton_->set_listener(on_animation_state_event, self);
        }
        return 0;
    }

    auto Skeleton::set_position(lua_State* L) -> int
    {
        return set1fv(L, [](::Skeleton* skeleton, const Vec2f& position) {
            skeleton->set_position(position);
        });
    }

    auto Skeleton::set_skin(lua_State* L) -> int
    {
        rainbow::lua::checkargs<Skeleton, rainbow::lua::nil_or<char*>>(L);

        Skeleton* self = Bind::self(L);
        if (self == nullptr)
            return 0;

        self->skeleton_->set_skin(lua_tostring(L, 2));
        return 0;
    }

    auto Skeleton::set_time_scale(lua_State* L) -> int
    {
        return set1f(L, [](::Skeleton* skeleton, float scale) {
            skeleton->set_time_scale(scale);
        });
    }

    void Skeleton::move_impl(const Vec2f& delta)
    {
        skeleton_->skeleton()->x += delta.x;
        skeleton_->skeleton()->y += delta.y;
    }

    void Skeleton::draw_impl() { skeleton_->draw(); }

    void Skeleton::update_impl(uint64_t dt)
    {
        skeleton_->update(dt);
    }
}}  // namespace spine::lua

#endif  // USE_LUA_SCRIPT
