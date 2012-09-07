#ifndef SHADER_H_
#define SHADER_H_

#include "Common/NonCopyable.h"

/// A shader is a GPU program for calculating rendering effects.
///
/// \see http://developer.apple.com/library/ios/#documentation/3DDrawing/Conceptual/OpenGLES_ProgrammingGuide/BestPracticesforShaders/BestPracticesforShaders.html#//apple_ref/doc/uid/TP40008793-CH7-SW3
///
/// Copyright 2012 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen
class Shader : public NonCopyable<Shader>
{
	friend class Pipeline;

public:
	/// Create a shader.
	/// \param type    Shader type (vertex or fragment).
	/// \param shader  Shader source code.
	Shader(const unsigned int type, const char *shader);
	~Shader();

	/// Return whether the shader was successfully compiled.
	operator bool() const;

private:
	unsigned int name;  ///< Name of the shader.
	unsigned int type;  ///< Shader type (vertex or fragment).
};

#endif
