#pragma once

#include "Flux/Renderer/Shader.h"

namespace Flux {

	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& vertexShaderSource, const std::string& fragmentShaderSource);
		virtual ~OpenGLShader();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetUniformMatrix4x4(const std::string& name, const float* data) const override;
	private:
		uint32 m_ProgramID = 0;
	};

}