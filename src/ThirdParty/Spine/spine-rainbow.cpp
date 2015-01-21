// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifdef USE_SPINE

#include "ThirdParty/Spine/spine-rainbow.h"

#include <algorithm>

#include <spine/SkeletonJson.h>
#include <spine/extension.h>

#include "Common/DataMap.h"
#include "FileSystem/Path.h"
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

	void compute_world_vertices(spMeshAttachment *self,
	                            spSlot *slot,
	                            float *vertices)
	{
		spMeshAttachment_computeWorldVertices(self, slot, vertices);
	}

	void compute_world_vertices(spSkinnedMeshAttachment *self,
	                            spSlot *slot,
	                            float *vertices)
	{
		spSkinnedMeshAttachment_computeWorldVertices(self, slot, vertices);
	}

	template<typename F>
	F for_each(const spSkeleton *skeleton, F &&f)
	{
		return std::move(
		    std::for_each(skeleton->drawOrder,
		                  skeleton->drawOrder + skeleton->slotsCount,
		                  std::forward<F>(f)));
	}

	template<typename T>
	TextureAtlas* get_texture(const T *attachment)
	{
		return static_cast<TextureAtlas*>(
		    static_cast<spAtlasRegion*>(attachment->rendererObject)->
		        page->rendererObject);
	}

	size_t get_vertex_count(const spSkeleton *skeleton, TextureAtlas **atlas)
	{
		size_t num_vertices = 0;
		TextureAtlas *texture = nullptr;
		for_each(skeleton, [&num_vertices, &texture](const spSlot *slot) {
			if (!slot->attachment)
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
					spMeshAttachment *mesh =
					    reinterpret_cast<spMeshAttachment*>(slot->attachment);
					num_vertices += mesh->trianglesCount;
					texture = get_texture(mesh);
					break;
				}
				case SP_ATTACHMENT_SKINNED_MESH: {
					spSkinnedMeshAttachment *mesh =
					    reinterpret_cast<spSkinnedMeshAttachment*>(
					        slot->attachment);
					num_vertices += mesh->trianglesCount;
					texture = get_texture(mesh);
					break;
				}
			}
		});
		if (atlas)
			*atlas = texture;
		return num_vertices;
	}

	void on_animation_state_event(spAnimationState *state,
	                              int track,
	                              spEventType type,
	                              spEvent *event,
	                              int loop_count)
	{
		using spine::lua::Skeleton;
		Skeleton *sk = static_cast<Skeleton*>(state->rendererObject);
		if (!sk->listener())
			return;

		sk->listener().get();
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
		if (event)
		{
			R_ASSERT(type == SP_ANIMATION_EVENT,
			         "Should only occur on animation events");

			lua_pushstring(sk->state(),
			               (!event->data) ? nullptr : event->data->name);
			++nargs;
		}
		lua_pushinteger(sk->state(), loop_count);
		rainbow::lua::call(
		    sk->state(), nargs, 0, 0,
		    "An error occurred while handling an animation state event");
	}

	template<typename T>
	int update_mesh(SpriteVertex *vertices,
	                const spSkeleton *skeleton,
	                T *mesh,
	                spSlot *slot)
	{
		if (mesh->trianglesCount > g_buffer_size)
		{
			g_buffer_size = mesh->trianglesCount * 2;
			g_buffer.reset(new float[g_buffer_size]);
		}
		float *coordinates = g_buffer.get();
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
	void _spAtlasPage_createTexture(spAtlasPage *self, const char *path)
	{
		const DataMap data{Path(path)};
		if (!data)
			return;
		TextureAtlas *texture = new TextureAtlas(data);
		if (!texture->is_valid())
			return;
		self->rendererObject = texture;
		self->width = texture->width();
		self->height = texture->height();
	}

	void _spAtlasPage_disposeTexture(spAtlasPage *self)
	{
		delete static_cast<TextureAtlas*>(self->rendererObject);
	}

	char* _spUtil_readFile(const char *path, int *length)
	{
		const DataMap data{Path(path)};
		*length = static_cast<int>(data.size());
		char *blob = new char[data.size()];
		memcpy(blob, data, data.size());
		return blob;
	}
}

Skeleton* Skeleton::from_json(const char *path, const float scale)
{
	/// Copy the path and replace ".json" with ".atlas"
	const size_t length = strlen(path);
	std::unique_ptr<char[]> atlas_path(new char[length + 7]);
	strcpy(atlas_path.get(), path);
	char *ext;
	if (path[length - 5] != '.')
	{
		ext = strrchr(atlas_path.get(), '.');
		if (!ext)
			ext = atlas_path.get() + length;
	}
	else
		ext = atlas_path.get() + length - 5;
	memcpy(ext, ".atlas\0", 7);

	spAtlas *atlas = spAtlas_createFromFile(atlas_path.get(), nullptr);
	atlas_path.reset();
	spSkeletonJson *json = spSkeletonJson_create(atlas);
	json->scale = scale;
	spSkeletonData *data = spSkeletonJson_readSkeletonDataFile(json, path);
	spSkeletonJson_dispose(json);
	if (!data)
	{
		LOGE("Spine: %s", json->error);
		return nullptr;
	}
	return new Skeleton(data, atlas);
}

Skeleton::Skeleton(spSkeletonData *data, spAtlas *atlas)
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

void Skeleton::set_flip(const bool x, const bool y)
{
	skeleton_->flipX = x;
	skeleton_->flipY = y;
}

void Skeleton::set_listener(spAnimationStateListener listener, void *self)
{
	state_->listener = listener;
	state_->rendererObject = self;
}

void Skeleton::set_position(const Vec2f &p)
{
	skeleton_->x = p.x;
	skeleton_->y = p.y;
}

void Skeleton::add_animation(const int track,
                             const char *animation,
                             const bool loop,
                             const float delay)
{
	spAnimationState_addAnimationByName(state_, track, animation, loop, delay);
}

void Skeleton::bind_textures() const
{
	texture_->bind();
}

void Skeleton::clear_track(const int track)
{
	spAnimationState_clearTrack(state_, track);
}

void Skeleton::clear_tracks()
{
	spAnimationState_clearTracks(state_);
}

const char* Skeleton::get_current_animation(const int track)
{
	spTrackEntry *entry = spAnimationState_getCurrent(state_, track);
	return (!entry) ? nullptr : entry->animation->name;
}

const char* Skeleton::get_skin()
{
	return skeleton_->skin->name;
}

void Skeleton::set_animation(const int track,
                             const char *animation,
                             const bool loop)
{
	spAnimationState_setAnimationByName(state_, track, animation, loop);
}

void Skeleton::set_animation_mix(const char *from,
                                 const char *to,
                                 const float duration)
{
	spAnimationStateData_setMixByName(animation_data_, from, to, duration);
}

void Skeleton::set_attachment(const char *slot, const char *attachment)
{
	spSkeleton_setAttachment(skeleton_, slot, attachment);
}

void Skeleton::set_skin(const char *skin)
{
	spSkeleton_setSkinByName(skeleton_, skin);
	spSkeleton_setToSetupPose(skeleton_);
}

void Skeleton::draw()
{
	Renderer::Get()->draw_arrays(
	    *this, 0, num_vertices_ + num_vertices_ / 2);
}

void Skeleton::update(const unsigned long dt)
{
	const float delta = dt / 1000.0f;
	spSkeleton_update(skeleton_, delta);
	spAnimationState_update(state_, delta * time_scale_);
	spAnimationState_apply(state_, skeleton_);
	spSkeleton_updateWorldTransform(skeleton_);

	// This solution is not ideal. We iterate over the bones twice: First to
	// determine number of vertices, second to update the vertex buffer.
	const size_t vertex_count = get_vertex_count(skeleton_, &texture_);
	if (vertex_count != num_vertices_)
	{
		num_vertices_ = vertex_count;
		if (vertex_count > max_vertices_)
		{
			max_vertices_ = vertex_count;
			vertices_.reset(new SpriteVertex[max_vertices_]);
		}
	}

	size_t i = 0;
	for_each(skeleton_, [this, &i](spSlot *slot) {
		if (!slot->attachment)
			return;
		switch (slot->attachment->type)
		{
			case SP_ATTACHMENT_REGION: {
				float vertices[8];
				spRegionAttachment *region =
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
				spMeshAttachment *mesh =
				    reinterpret_cast<spMeshAttachment*>(slot->attachment);
				R_ASSERT(texture_ == get_texture(mesh),
				         kErrorMultipleTexturesUnsupported);
				i += update_mesh(&vertices_[i], skeleton_, mesh, slot);
				break;
			}
			case SP_ATTACHMENT_SKINNED_MESH: {
				spSkinnedMeshAttachment *mesh =
				    reinterpret_cast<spSkinnedMeshAttachment*>(
				        slot->attachment);
				R_ASSERT(texture_ == get_texture(mesh),
				         kErrorMultipleTexturesUnsupported);
				i += update_mesh(&vertices_[i], skeleton_, mesh, slot);
				break;
			}
		}
		if (slot->data->additiveBlending)  // TODO: Implement.
			LOGE("Additive blending not yet implemented");
	});

	vertex_buffer_.upload(vertices_.get(), i * sizeof(SpriteVertex));
}

NS_RAINBOW_LUA_BEGIN
{
	using spine::lua::Skeleton;

	template<>
	const char Skeleton::Bind::class_name[] = "skeleton";

	template<>
	const bool Skeleton::Bind::is_constructible = true;

	template<>
	const luaL_Reg Skeleton::Bind::functions[] = {
		{ "add_animation",          &Skeleton::add_animation },
		{ "clear_track",            &Skeleton::clear_track },
		{ "clear_tracks",           &Skeleton::clear_tracks },
		{ "get_current_animation",  &Skeleton::get_current_animation },
		{ "get_skin",               &Skeleton::get_skin },
		{ "set_animation",          &Skeleton::set_animation },
		{ "set_animation_mix",      &Skeleton::set_animation_mix },
		{ "set_attachment",         &Skeleton::set_attachment },
		{ "set_flip",               &Skeleton::set_flip },
		{ "set_listener",           &Skeleton::set_listener },
		{ "set_position",           &Skeleton::set_position },
		{ "set_skin",               &Skeleton::set_skin },
		{ "set_time_scale",         &Skeleton::set_time_scale },
		{ nullptr, nullptr }
	};
} NS_RAINBOW_LUA_END

namespace spine
{
	namespace lua
	{
		Skeleton::Skeleton(lua_State *L) : state_(L)
		{
			rainbow::lua::Argument<char*>::is_required(L, 1);
			rainbow::lua::Argument<lua_Number>::is_optional(L, 2);

			skeleton_.reset(
			    ::Skeleton::from_json(lua_tostring(L, 1),
			                          rainbow::lua::optnumber(L, 2, 1.0f)));
		}

		int Skeleton::add_animation(lua_State *L)
		{
			rainbow::lua::Argument<lua_Number>::is_required(L, 2);
			rainbow::lua::Argument<char*>::is_required(L, 3);
			rainbow::lua::Argument<bool>::is_required(L, 4);
			rainbow::lua::Argument<lua_Number>::is_required(L, 5);

			Skeleton *self = Bind::self(L);
			if (!self)
				return 0;

			self->skeleton_->add_animation(
			    lua_tointeger(L, 2),
			    lua_tostring(L, 3),
			    lua_toboolean(L, 4),
			    lua_tonumber(L, 5));
			return 0;
		}

		int Skeleton::clear_track(lua_State *L)
		{
			return set1i(L, [](::Skeleton *skeleton, const int track) {
				skeleton->clear_track(track);
			});
		}

		int Skeleton::clear_tracks(lua_State *L)
		{
			Skeleton *self = Bind::self(L);
			if (!self)
				return 0;

			self->skeleton_->clear_tracks();
			return 0;
		}

		int Skeleton::get_current_animation(lua_State *L)
		{
			rainbow::lua::Argument<lua_Number>::is_required(L, 2);

			Skeleton *self = Bind::self(L);
			if (!self)
				return 0;

			const char *name =
			    self->skeleton_->get_current_animation(lua_tointeger(L, 2));
			if (!name)
				return 0;
			lua_pushstring(L, name);
			return 1;
		}

		int Skeleton::get_skin(lua_State *L)
		{
			Skeleton *self = Bind::self(L);
			if (!self)
				return 0;

			lua_pushstring(L, self->skeleton_->get_skin());
			return 1;
		}

		int Skeleton::set_animation(lua_State *L)
		{
			rainbow::lua::Argument<lua_Number>::is_required(L, 2);
			rainbow::lua::Argument<char*>::is_required(L, 3);
			rainbow::lua::Argument<bool>::is_required(L, 4);

			Skeleton *self = Bind::self(L);
			if (!self)
				return 0;

			self->skeleton_->set_animation(
			    lua_tointeger(L, 2),
			    lua_tostring(L, 3),
			    lua_toboolean(L, 4));
			return 0;
		}

		int Skeleton::set_animation_mix(lua_State *L)
		{
			rainbow::lua::Argument<char*>::is_required(L, 2);
			rainbow::lua::Argument<char*>::is_required(L, 3);
			rainbow::lua::Argument<lua_Number>::is_required(L, 4);

			Skeleton *self = Bind::self(L);
			if (!self)
				return 0;

			self->skeleton_->set_animation_mix(
			    lua_tostring(L, 2),
			    lua_tostring(L, 3),
			    lua_tonumber(L, 4));
			return 0;
		}

		int Skeleton::set_attachment(lua_State *L)
		{
			rainbow::lua::Argument<char*>::is_required(L, 2);
			rainbow::lua::Argument<char*>::is_optional(L, 3);

			Skeleton *self = Bind::self(L);
			if (!self)
				return 0;

			self->skeleton_->set_attachment(lua_tostring(L, 2),
			                                lua_tostring(L, 3));
			return 0;
		}

		int Skeleton::set_flip(lua_State *L)
		{
			rainbow::lua::Argument<bool>::is_required(L, 2);
			rainbow::lua::Argument<bool>::is_required(L, 3);

			Skeleton *self = Bind::self(L);
			if (!self)
				return 0;

			self->skeleton_->set_flip(lua_toboolean(L, 2), lua_toboolean(L, 3));
			return 0;
		}

		int Skeleton::set_listener(lua_State *L)
		{
			rainbow::lua::Argument<void*>::is_optional(L, 2);

			Skeleton *self = Bind::self(L);
			if (!self)
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

		int Skeleton::set_position(lua_State *L)
		{
			return set1fv(L, [](::Skeleton *skeleton, const Vec2f &position) {
				skeleton->set_position(position);
			});
		}

		int Skeleton::set_skin(lua_State *L)
		{
			rainbow::lua::Argument<char*>::is_optional(L, 2);

			Skeleton *self = Bind::self(L);
			if (!self)
				return 0;

			self->skeleton_->set_skin(lua_tostring(L, 2));
			return 0;
		}

		int Skeleton::set_time_scale(lua_State *L)
		{
			return set1f(L, [](::Skeleton *skeleton, const float scale) {
				skeleton->set_time_scale(scale);
			});
		}

		void Skeleton::move_impl(const Vec2f &delta)
		{
			skeleton_->skeleton()->x += delta.x;
			skeleton_->skeleton()->y += delta.y;
		}

		void Skeleton::draw_impl()
		{
			skeleton_->draw();
		}

		void Skeleton::update_impl(const unsigned long dt)
		{
			skeleton_->update(dt);
		}
	}
}

#endif
