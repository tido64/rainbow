// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_SHADERMANAGER_H_
#define GRAPHICS_SHADERMANAGER_H_

#include <vector>

#include "Common/Global.h"
#include "Common/Vec2.h"
#include "Graphics/OpenGL.h"
#include "Graphics/ShaderDetails.h"

class Renderer;

class ShaderManager : public Global<ShaderManager>
{
	friend Renderer;

public:
	enum
	{
		kInvalidProgram = 0,
		kDefaultProgram
	};

	/// Saves the current shader context and restores it when exiting scope.
	class Context
	{
	public:
		Context() : program_(ShaderManager::Get()->current_) {}
		~Context() { ShaderManager::Get()->use(program_); }

	private:
		unsigned int program_;
	};

	/// Compiles program.
	/// \param shaders     Shader parameters.
	/// \param attributes  Shader attributes.
	/// \return Unique program identifier; kInvalidProgram if unsuccessful.
	unsigned int compile(Shader::Params *shaders,
	                     const Shader::AttributeParams *attributes);

	/// Returns current program details.
	inline const Shader::Details& get_program() const;

	/// Returns program details.
	inline Shader::Details& get_program(const unsigned int pid);

	/// Updates orthographic projection.
	void update_projection();

	/// Updates viewport.
	void update_viewport();

	/// Activates program.
	void use(const unsigned int program);

private:
	unsigned int current_;                   ///< Currently used program.
	Renderer *renderer_;
	std::vector<Shader::Details> programs_;  ///< Linked shader programs.
	std::vector<unsigned int> shaders_;      ///< Compiled shaders.

	ShaderManager(Renderer *);
	~ShaderManager();

	bool init();
};

const Shader::Details& ShaderManager::get_program() const
{
	R_ASSERT(current_ > 0, "ShaderManager is uninitialised");
	return programs_[current_ - 1];
}

Shader::Details& ShaderManager::get_program(const unsigned int pid)
{
	R_ASSERT(pid > 0, "Invalid shader program id");
	return programs_[pid - 1];
}

#endif
