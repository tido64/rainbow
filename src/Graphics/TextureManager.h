// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_TEXTUREMANAGER_H_
#define GRAPHICS_TEXTUREMANAGER_H_

#include "Common/Global.h"
#include "Common/Vector.h"

#define RECORD_VMEM_USAGE !defined(NDEBUG) || defined(USE_HEIMDALL)

/// Manages texture resources.
///
/// Generates and reuses texture ids whenever possible. This should eliminate
/// problems caused by drivers that return deleted ids before they are actually
/// deleted. This causes black textures.
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
	/// Makes texture active on current rendering target.
	/// \param id  Texture id to bind. If omitted, bind the default texture.
	void bind(const unsigned int id = 0);

	/// Makes texture active on specified unit.
	/// \param id    Texture id to bind.
	/// \param unit  Texture unit to bind on.
	void bind(const unsigned int id, const unsigned int unit);

	/// Creates a texture.
	/// \param internal_format  Preferred renderer's internal format.
	/// \param width            Width of the texture.
	/// \param height           Height of the texture.
	/// \param format           Format of the bitmap data.
	/// \param data             Bitmap data.
	/// \return Texture id.
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
	/// \return Texture id.
	unsigned int create_compressed(const unsigned int format,
	                               const unsigned int width,
	                               const unsigned int height,
	                               const size_t size,
	                               const void *data);

#if RECORD_VMEM_USAGE
	/// Returns total video memory used (and unused) by textures.
	void memory_usage(double &used, double &unused, double &peak) const;
#endif

	/// Purges unused texture memory.
	inline void purge();

	/// Deletes texture. In reality, the texture is put in a recycle bin and may
	/// be reused unless purged.
	/// \param id  Texture to delete.
	void remove(const unsigned int id);

	void set_filter(const int filter);

private:
	static const size_t kNumTextureUnits = 2;

	/// Structure storing a texture's id and area.
	struct TextureId
	{
		unsigned int id;  ///< Texture id.
		unsigned int sz;  ///< Texture area.

		friend bool operator==(const TextureId &a, const TextureId &b)
		{
			return a.id == b.id;
		}

		friend bool operator>(const TextureId &a, const TextureId &b)
		{
			return a.sz > b.sz;
		}
	};

	unsigned int active_[kNumTextureUnits];
	Vector<TextureId> textures_;  ///< Stores texture ids currently in use.
	Vector<TextureId> recycled_;  ///< Stores reusable texture ids.
	int mag_filter_;
	int min_filter_;

#if RECORD_VMEM_USAGE
	double mem_peak_;
	double mem_used_;
#endif

	TextureManager();
	~TextureManager();

	/// Returns a recycled texture id or creates a new one, ready for upload.
	TextureId create_texture(const unsigned int size);

	/// Prints total video memory used by textures.
	void print_usage() const;

	/// Clears and deletes textures.
	void purge(Vector<TextureId> &textures);
};

void TextureManager::purge()
{
	purge(recycled_);
}

#endif
