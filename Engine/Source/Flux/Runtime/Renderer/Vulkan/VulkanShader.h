#pragma once

#include "Flux/Runtime/Renderer/Shader.h"

#include "Vulkan.h"

namespace Flux {

	class VulkanShader : public Shader
	{
	public:
		VulkanShader(const std::filesystem::path& path);
		virtual ~VulkanShader();

		virtual void Reload() override;

		const std::unordered_map<ShaderStage, VkShaderModule>& GetShaderModules() const { return m_ShaderModules; }
	private:
		ShaderBinaryMap LoadOrCompileBinaries(const ShaderSourceMap& sources);

		void RT_CreateShaders();
	private:
		std::filesystem::path m_Path;
		ShaderBinaryMap m_Binaries;
		std::unordered_map<ShaderStage, VkShaderModule> m_ShaderModules;
	};

}