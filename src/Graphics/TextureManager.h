// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_TEXTUREMANAGER_H_
#define GRAPHICS_TEXTUREMANAGER_H_

#include <vector>

#include "Common/Global.h"

#define RECORD_VMEM_USAGE !defined(NDEBUG) || defined(USE_HEIMDALL)

/// Manages texture resources.
///
/// Generates and reuses texture names whenever possible. This should eliminate
/// problems caused by drivers that return deleted names before they are
/// actually deleted. This causes black textures.
///
/// Deleted textures are shelved until they are needed again. In order to
/// minimize memory use, they are resized to a minimal texture dimension. Of
/// course, it is also possible to purge this storage by issuing \c purge(). To
/// avoid problems, this should only occur when you're done loading textures
/// for a while.
class TextureManager : public Global<TextureManager>
{
	friend class Renderer;

public:
#if RECORD_VMEM_USAGE
	struct MemoryUsage
	{
		double used;
		double peak;
	};

	/// Returns total video memory used by textures.
	MemoryUsage memory_usage() const;
#endif

	void set_filter(const int filter);

	/// Makes texture active on current rendering target.
	/// \param name  Name of texture. If omitted, binds the default texture.
	void bind(const unsigned int name = 0);

	/// Makes texture active on specified unit.
	/// \param name  Name of texture.
	/// \param unit  Texture unit to bind on.
	void bind(const unsigned int name, const unsigned int unit);

	/// Generates an empty texture.
	unsigned int create();

	/// Creates a texture.
	/// \param internal_format  Preferred renderer's internal format.
	/// \param width            Width of the texture.
	/// \param height           Height of the texture.
	/// \param format           Format of the bitmap data.
	/// \param data             Bitmap data.
	/// \return Texture name.
	unsigned int create(const unsigned int internal_format,
	                    const unsigned int width,
	                    const unsigned int height,
	                    const unsigned int format,
	                    const void *data);

	/// Creates texture from a compressed format.
	/// \param format  Compression format.
	/// \param width   Width of the texture.
	/// \param height  Height of the texture.
	/// \param size    Data size.
	/// \param data    Compressed bitmap data.
	/// \return Texture name.
	unsigned int create_compressed(const unsigned int format,
	                               const unsigned int width,
	                               const unsigned int height,
	                               const size_t size,
	                               const void *data);

	/// Purges unused texture memory.
	void purge();

	/// Deletes texture.
	/// \param name  Name of texture to delete.
	void remove(const unsigned int name);

	/// Uploads bitmap data to specified texture.
	/// \param name             Name of target texture.
	/// \param internal_format  Preferred renderer's internal format.
	/// \param width            Width of the texture.
	/// \param height           Height of the texture.
	/// \param format           Format of the bitmap data.
	/// \param data             Bitmap data.
	void upload(const unsigned int name,
	            const unsigned int internal_format,
	            const unsigned int width,
	            const unsigned int height,
	            const unsigned int format,
	            const void *data);

private:
#if RECORD_VMEM_USAGE
	struct TextureName
	{
		unsigned int name;
		unsigned int size;

		TextureName(const unsigned int name_) : name(name_), size(0) {}

		operator unsigned int() const { return name; }

		friend bool operator==(const TextureName &t, const unsigned int name)
		{
			return t.name == name;
		}
	};
#else
	using TextureName = unsigned int;
#endif

	static const size_t kNumTextureUnits = 2;

	unsigned int active_[kNumTextureUnits];
	std::vector<TextureName> textures_;   ///< Stores texture names currently in use.
	std::vector<unsigned int> recycled_;  ///< Stores reusable texture names.
	int mag_filter_;
	int min_filter_;

#if RECORD_VMEM_USAGE
	double mem_peak_;
	double mem_used_;
#endif

	TextureManager();
	~TextureManager() = default;

#if RECORD_VMEM_USAGE
	/// Prints total video memory used by textures.
	void print_usage() const;

	/// Records usage of texture memory.
	void record_usage(TextureName &name, const unsigned int size);
#endif
};

#endif
