#ifndef GRAPHICS_PIPELINE_H_
#define GRAPHICS_PIPELINE_H_

#include "Common/NonCopyable.h"

class Shader;

/// A pipeline is a combination of a vertex and a fragment shader.
///
/// Copyright 2012 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen
class Pipeline : public NonCopyable<Pipeline>
{
public:
	enum Attribute
	{
		COLOR_LOCATION,
		TEXCOORD_LOCATION,
		VERTEX_LOCATION
	};

	Pipeline();
	~Pipeline();

	/// Attach a shader to the program.
	/// \param shader  The shader to attach.
	void attach_shader(const Shader *shader);

	/// Link the program.
	/// \return \c true on success, \c false otherwise.
	bool link();

	/// Install the program as part of current rendering state.
	void use() const;

	/// Return the program name. 0 if an error has occurred.
	inline operator unsigned int() const;

private:
	unsigned int program;   ///< Program name.
	unsigned int vertex;    ///< Attached vertex shader.
	unsigned int fragment;  ///< Attached fragment shader.
};

Pipeline::operator unsigned int() const
{
	return this->program;
}

#endif
