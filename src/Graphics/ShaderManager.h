// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_SHADERMANAGER_H_
#define GRAPHICS_SHADERMANAGER_H_

#include <array>
#include <vector>

#include "Common/Global.h"
#include "Common/Vec2.h"
#include "Graphics/OpenGL.h"
#include "Graphics/ShaderDetails.h"

class ShaderManager : public Global<ShaderManager>
{
	friend class Renderer;

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

	/// Sets viewport.
	void set(const Vec2i &);

	/// Sets orthographic projection.
	void set_projection(const float left,
	                    const float right,
	                    const float bottom,
	                    const float top);

	/// Activates program.
	void use(const unsigned int program);

private:
	unsigned int current_;                   ///< Currently used program.
	std::vector<Shader::Details> programs_;  ///< Linked shader programs.

	/// The orthographic projection matrix is defined as:
	///   | 2 / (r - l)        0             0       -(r + l) / (r - l) |
	///   |      0        2 / (t - b)        0       -(t + b) / (t - b) |
	///   |      0             0       -2 / (f - n)  -(f + n) / (f - n) |
	///   |      0             0             0                0         |
	/// Where b = bottom, f = far, l = left, n = near, r = right, t = top, and
	/// near = -1.0 and far = 1.0. The matrix is stored in column-major order.
	std::array<float, 16> ortho_;

	std::vector<unsigned int> shaders_;  ///< Compiled shaders.

	ShaderManager();
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
