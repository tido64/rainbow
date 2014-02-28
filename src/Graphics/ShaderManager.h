// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_SHADERMANAGER_H_
#define GRAPHICS_SHADERMANAGER_H_

#include "Common/Vec2.h"
#include "Common/Vector.h"
#include "Graphics/OpenGL.h"
#include "Graphics/ShaderDetails.h"

class ShaderManager : private NonCopyable<ShaderManager>
{
	friend class Renderer;

public:
	/// Saves the current shader context and restores it when exiting scope.
	class Context
	{
	public:
		inline Context();
		inline ~Context();

	private:
		int program;
	};

	static ShaderManager *Instance;

	/// Compiles program.
	/// \param shaders     Shader parameters.
	/// \param attributes  Shader attributes.
	/// \return Unique program identifier; -1 if unsuccessful.
	int compile(Shader::ShaderParams *shaders,
	            const Shader::AttributeParams *attributes);

	/// Returns current program details.
	inline Shader::Details& get_program() const;

	/// Returns program details.
	inline Shader::Details& get_program(const int pid) const;

	/// Sets viewport.
	void set(const Vec2i &);

	/// Sets orthographic projection.
	void set_projection(const float left, const float right,
	                    const float bottom, const float top);

	/// Activates program.
	void use(const int program);

private:
	int active;  ///< Currently active program.

	Vector<unsigned int> shaders;
	Vector<Shader::Details> programs;

	float ortho[16];

	ShaderManager();
	~ShaderManager();

	void init();
};

ShaderManager::Context::Context() : program(ShaderManager::Instance->active) { }

ShaderManager::Context::~Context()
{
	ShaderManager::Instance->use(this->program);
}

Shader::Details& ShaderManager::get_program() const
{
	R_ASSERT(this->active >= 0, "ShaderManager is uninitialised");
	return this->get_program(this->active);
}

Shader::Details& ShaderManager::get_program(const int pid) const
{
	R_ASSERT(pid >= 0, "Invalid shader program id");
	return this->programs[pid];
}

#endif
