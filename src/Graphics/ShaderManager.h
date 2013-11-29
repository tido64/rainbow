// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_SHADERMANAGER_H_
#define GRAPHICS_SHADERMANAGER_H_

#include "Common/Vector.h"
#include "Graphics/OpenGL.h"
#include "Graphics/ShaderDetails.h"

namespace Renderer
{
	bool init();
	void release();
}

class ShaderManager : private NonCopyable<ShaderManager>
{
	friend bool Renderer::init();
	friend void Renderer::release();

public:
	static ShaderManager *Instance;

	/// Creates program.
	/// \param shaders  Shader ids to attach.
	/// \param count    Number of shader ids.
	/// \return Unique program identifier.
	int create_program(const int *shaders, const size_t count);

	/// Creates shader.
	/// \param type    Shader type.
	/// \param source  Shader source code.
	/// \return Unique shader identifier.
	int create_shader(int type, const char *source);

	/// Returns program details.
	inline Shader::Details& get_program(const int pid) const;

	/// Sets viewport.
	/// TODO: Setting the viewport after changing the projection won't work. For
	///       now, this function works only because we don't change resolution
	///       on the devices.
	void set(const float width, const float height);

	/// Sets orthographic projection.
	void set_projection(const float left, const float right,
	                    const float bottom, const float top);

	/// Activates program.
	void use(const int program);

	/// Returns whether ShaderManager was properly constructed.
	inline operator bool() const;

private:
	int active;  ///< Currently active program.

	Vector<unsigned int> shaders;
	Vector<Shader::Details> programs;

	float ortho[16];

	ShaderManager(const char *shaders[2]);
	~ShaderManager();

	/// Initialises shaders. Run when setting the viewport for the first time.
	void initialise();
};

Shader::Details& ShaderManager::get_program(const int pid) const
{
	R_ASSERT(pid >= 0, "Invalid shader program id");
	return this->programs[pid];
}

ShaderManager::operator bool() const
{
	return this->programs.size();
}

#endif
