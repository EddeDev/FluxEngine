#pragma once

#include "Flux/Runtime/Renderer/Shader.h"

namespace Flux {

	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::filesystem::path& path);
		OpenGLShader(const std::string& vertexShaderSource, const std::string& fragmentShaderSource);
		virtual ~OpenGLShader();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetUniform(const std::string& name, const Vector3& value) const override;
		virtual void SetUniform(const std::string& name, const Vector4& value) const override;
		virtual void SetUniform(const std::string& name, const Matrix4x4& value) const override;
	private:
		struct OpenGLShaderData
		{
			uint32 ProgramID;
		};

		OpenGLShaderData* m_Data = nullptr;
	};

}