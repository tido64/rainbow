// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_TEXTUREMANAGER_H_
#define GRAPHICS_TEXTUREMANAGER_H_

#include <vector>

#include "Common/Global.h"

#define RECORD_VMEM_USAGE !defined(NDEBUG) || defined(USE_HEIMDALL)

/// <summary>Manages texture resources.</summary>
/// <remarks>
///   <para>
///     Generates and reuses texture names whenever possible. This should
///     eliminate problems caused by drivers that return deleted names before
///     they are actually deleted. This causes black textures.
///   </para>
///   <para>
///     Deleted textures are shelved until they are needed again. In order to
///     minimize memory use, they are resized to a minimal texture dimension. Of
///     course, it is also possible to purge this storage by issuing
///     <see cref="TextureManager::purge"/>. To avoid problems, this should only
///     occur when you're done loading textures for a while.
///   </para>
/// </remarks>
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

	/// <summary>Returns total video memory used by textures.</summary>
	MemoryUsage memory_usage() const;
#endif

	void set_filter(const int filter);

	/// <summary>Makes texture active on current rendering target.</summary>
	/// <param name="name">
	///   Name of texture. If omitted, binds the default texture.
	/// </param>
	void bind(const unsigned int name = 0);

	/// <summary>Makes texture active on specified unit.</summary>
	/// <param name="name">Name of texture.</param>
	/// <param name="unit">Texture unit to bind on.</param>
	void bind(const unsigned int name, const unsigned int unit);

	/// <summary>Generates an empty texture.</summary>
	unsigned int create();

	/// <summary>Creates a texture.</summary>
	/// <param name="internal_format">Internal format of the texture.</param>
	/// <param name="width">Width of the texture.</param>
	/// <param name="height">Height of the texture.</param>
	/// <param name="format">Format of the bitmap data.</param>
	/// <param name="data">Bitmap data.</param>
	/// <returns>Texture name.</returns>
	unsigned int create(const unsigned int internal_format,
	                    const unsigned int width,
	                    const unsigned int height,
	                    const unsigned int format,
	                    const void *data);

	/// <summary>Creates texture from a compressed format.</summary>
	/// <param name="format">Compression format.</param>
	/// <param name="width">Width of the texture.</param>
	/// <param name="height">Height of the texture.</param>
	/// <param name="size">Data size.</param>
	/// <param name="data">Compressed bitmap data.</param>
	/// <returns>Texture name.</returns>
	unsigned int create_compressed(const unsigned int format,
	                               const unsigned int width,
	                               const unsigned int height,
	                               const size_t size,
	                               const void *data);

	/// <summary>Purges unused texture memory.</summary>
	void purge();

	/// <summary>Deletes texture.</summary>
	/// <param name="name">Name of texture to delete.</param>
	void remove(const unsigned int name);

	/// <summary>Uploads bitmap data to specified texture.</summary>
	/// <param name="name">Name of target texture.</param>
	/// <param name="internal_format">Internal format of the texture.</param>
	/// <param name="width">Width of the texture.</param>
	/// <param name="height">Height of the texture.</param>
	/// <param name="format">Format of the bitmap data.</param>
	/// <param name="data">Bitmap data.</param>
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
	/// <summary>Prints total video memory used by textures.</summary>
	void print_usage() const;

	/// <summary>Records usage of texture memory.</summary>
	void record_usage(TextureName &name, const unsigned int size);
#endif
};

#endif
