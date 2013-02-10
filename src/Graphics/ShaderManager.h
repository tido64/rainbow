#ifndef SHADERMANAGER_H_
#define SHADERMANAGER_H_

#include "Common/Vector.h"
#include "Graphics/OpenGL.h"

namespace Shader
{
	enum Attribute
	{
		VERTEX,
		COLOR,
		TEXCOORD
	};

	enum Type
	{
		FRAGMENT_SHADER = GL_FRAGMENT_SHADER,
		VERTEX_SHADER = GL_VERTEX_SHADER,
	#ifdef GL_GEOMETRY_SHADER
		GEOMETRY_SHADER = GL_GEOMETRY_SHADER,
	#endif
		INVALID = -1
	};
}

class ShaderManager : private NonCopyable<ShaderManager>
{
public:
	static ShaderManager *Instance;

	ShaderManager(const char **shaders = nullptr, const size_t count = 0);
	~ShaderManager();

	/// Return the real program id.
	inline unsigned int get_program(const unsigned int pid) const;

	/// Create program.
	/// \param shaders  Shader ids to attach.
	/// \param count    Number of shader ids.
	/// \return Unique program identifier.
	int create_program(const int *shaders, const size_t count);

	/// Create shader.
	/// \param type    Shader type.
	/// \param source  Shader source code.
	/// \return Unique shader identifier.
	int create_shader(int type, const char *source);

	/// Reset to default shaders.
	inline void reset();

	/// Set viewport.
	void set(const float width, const float height);

	/// Set orthographic projection.
	void set_ortho(const float left, const float right, const float bottom, const float top);

	/// Use program.
	void use(const int program);

	inline operator bool() const;

private:
	struct Program
	{
		unsigned int id;
	} active;  ///< Currently active program.

	Vector<unsigned int> shaders;
	Vector<Program> programs;

	float ortho[16];
};

unsigned int ShaderManager::get_program(const unsigned int pid) const
{
	return this->programs[pid].id;
}

void ShaderManager::reset()
{
	this->use(0);
}

ShaderManager::operator bool() const
{
	return this->programs.size();
}

#endif
