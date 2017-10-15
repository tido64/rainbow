// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "ThirdParty/Spine/spine-rainbow.h"

#include <algorithm>

#include <spine/SkeletonJson.h>
#include <spine/extension.h>

#include "Common/Data.h"
#include "FileSystem/File.h"
#include "Graphics/Renderer.h"
#include "Graphics/SpriteVertex.h"
#include "Graphics/TextureAtlas.h"

using rainbow::Data;
using rainbow::File;
using rainbow::FileType;
using rainbow::SpriteVertex;
using rainbow::TextureAtlas;
using rainbow::Vec2f;
using rainbow::czstring;

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
    void _spAtlasPage_createTexture(spAtlasPage* self, czstring path)
    {
        auto texture = std::make_unique<TextureAtlas>(path);
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

    auto _spUtil_readFile(czstring path, int* length) -> char*
    {
        const Data& data = File::read(path, FileType::Asset);
        *length = static_cast<int>(data.size());
        auto blob = std::make_unique<char[]>(data.size());
        memcpy(blob.get(), data.bytes(), data.size());
        return blob.release();
    }
}  // extern "C"

auto Skeleton::from_json(czstring path, float scale) -> Skeleton*
{
    // Copy the path and replace ".json" with ".atlas"
    const size_t length = strlen(path);
    czstring ext = path + length - 5;
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
        const bool no_message = json->error == nullptr || *json->error == '\0';
        LOGE("Spine: %s", no_message ? "No skeleton data" : json->error);
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
    skeleton_->flipX = static_cast<int>(x);
    skeleton_->flipY = static_cast<int>(y);
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
                             czstring animation,
                             bool loop,
                             float delay)
{
    spAnimationState_addAnimationByName(
        state_, track, animation, static_cast<int>(loop), delay);
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

auto Skeleton::get_current_animation(int track) -> czstring
{
    spTrackEntry* entry = spAnimationState_getCurrent(state_, track);
    return entry == nullptr ? nullptr : entry->animation->name;
}

auto Skeleton::get_skin() -> czstring
{
    return skeleton_->skin->name;
}

void Skeleton::set_animation(int track, czstring animation, bool loop)
{
    spAnimationState_setAnimationByName(
        state_, track, animation, static_cast<int>(loop));
}

void Skeleton::set_animation_mix(czstring from,
                                 czstring to,
                                 float duration)
{
    spAnimationStateData_setMixByName(animation_data_, from, to, duration);
}

void Skeleton::set_attachment(czstring slot, czstring attachment)
{
    spSkeleton_setAttachment(skeleton_, slot, attachment);
}

void Skeleton::set_skin(czstring skin)
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
