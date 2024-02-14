#pragma once

namespace Flux {

	enum class ShaderStage : uint8
	{
		None = 0,

		Vertex,
		Fragment,
		Compute
	};

	using ShaderSourceMap = std::unordered_map<ShaderStage, std::string>;

	class Shader : public ReferenceCounted
	{
	public:
		virtual ~Shader() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetUniform(const std::string& name, float value) const = 0;
		virtual void SetUniform(const std::string& name, int32 value) const = 0;
		virtual void SetUniform(const std::string& name, uint32 value) const = 0;
		virtual void SetUniform(const std::string& name, const Vector2& value) const = 0;
		virtual void SetUniform(const std::string& name, const Vector3& value) const = 0;
		virtual void SetUniform(const std::string& name, const Vector4& value) const = 0;
		virtual void SetUniform(const std::string& name, const Matrix4x4& value) const = 0;

		static Ref<Shader> Create(const std::filesystem::path& path);
		static Ref<Shader> Create(const std::string& vertexShaderSource, const std::string& fragmentShaderSource);
	};

}