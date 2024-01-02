#pragma once

namespace Flux {

	enum class ShaderStage : uint8
	{
		Vertex,
		Fragment,
		Compute
	};

	class Shader : public ReferenceCounted
	{
	public:
		virtual ~Shader() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetUniformMatrix4x4(const std::string& name, const float* data) const = 0;

		static Ref<Shader> Create(const std::string& vertexShaderSource, const std::string& fragmentShaderSource);
	};

}