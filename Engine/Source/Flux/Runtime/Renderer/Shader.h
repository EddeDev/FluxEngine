#pragma once

namespace Flux {

	enum class ShaderStage : uint8
	{
		None = 0,

		Vertex,
		Fragment,
		Compute
	};

	typedef std::vector<uint32> SPIRVBinary;
	typedef std::unordered_map<ShaderStage, SPIRVBinary> ShaderBinaryMap;
	typedef std::unordered_map<ShaderStage, std::string> ShaderSourceMap;

	class Shader : public ReferenceCounted
	{
	public:
		virtual void Reload() = 0;

		static Ref<Shader> Create(const std::filesystem::path& path);
	};

}