#ifndef TEXTUREMANAGER_H_
#define TEXTUREMANAGER_H_

#include "Common/Vector.h"

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
///
/// Copyright 2012 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen
class TextureManager
{
public:
	static inline TextureManager& Instance();

	~TextureManager();

	/// Make texture active on current rendering target.
	/// \param id  Texture id to bind. If omitted, bind the default texture.
	void bind(const unsigned int id = 0);

	/// Create a texture.
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

	/// Purge unused texture memory.
	inline void purge();

	/// Delete texture. In reality, the texture is put in a recycle bin and may
	/// be reused unless purged.
	/// \param id  Texture to delete.
	void remove(const unsigned int id);

private:
	/// Structure storing a texture's id and area.
	struct TextureId
	{
		unsigned int id;  ///< Texture id.
		unsigned int sz;  ///< Texture area.

		inline bool operator==(const TextureId &tex);
		inline bool operator>(const TextureId &tex);
	};

	unsigned int active;
	Vector<TextureId> recycled;  ///< Stores reusable texture ids.
	Vector<TextureId> textures;  ///< Stores texture ids currently in use.

	inline TextureManager();

	/// Intentionally left undefined.
	TextureManager(const TextureManager &);

	/// Print the total video memory used by textures.
	void print_usage() const;

	/// Clear and delete textures.
	void purge(Vector<TextureId> &textures);

	/// Intentionally left undefined.
	TextureManager& operator=(const TextureManager &);
};

TextureManager& TextureManager::Instance()
{
	static TextureManager manager;
	return manager;
}

void TextureManager::purge()
{
	this->purge(this->recycled);
}

TextureManager::TextureManager() : active(0) { }

bool TextureManager::TextureId::operator==(const TextureId &tex)
{
	return tex.id == this->id;
}

bool TextureManager::TextureId::operator>(const TextureId &tex)
{
	return this->sz > tex.sz;
}

#endif
