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

		virtual const VertexInputLayout& GetVertexInputLayout() const { return m_VertexInputLayout; }
		virtual const PushConstantMap& GetPushConstants() const { return m_PushConstants; }
		virtual const DescriptorSetMap& GetDescriptorSets() const { return m_DescriptorSets; }

		virtual const std::filesystem::path& GetPath() const { return m_Path; }

		VkDescriptorSet RT_CreateDescriptorSet(uint32 set) const;
		VkDescriptorSetLayout GetDescriptorSetLayout(uint32 set) const { return m_DescriptorSetLayouts.at(set); }

		const std::vector<VkDescriptorPoolSize>& GetDescriptorPoolSizes(uint32 set) const { return m_DescriptorPoolSizes.at(set); }
		const std::unordered_map<uint32, std::vector<VkDescriptorPoolSize>>& GetDescriptorPoolSizes() const { return m_DescriptorPoolSizes; }
		
		const std::unordered_map<uint32, VkDescriptorSetLayout>& GetDescriptorSetLayouts() const { return m_DescriptorSetLayouts; }
		const std::unordered_map<ShaderStage, VkShaderModule>& GetShaderModules() const { return m_ShaderModules; }
	private:
		ShaderBinaryMap LoadOrCompileBinaries(const ShaderSourceMap& sources);

		void RT_CreateShaders();
		void Reflect();
		void CreateDescriptors();
	private:
		std::filesystem::path m_Path;

		ShaderBinaryMap m_Binaries;
		
		VertexInputLayout m_VertexInputLayout;
		PushConstantMap m_PushConstants;
		DescriptorSetMap m_DescriptorSets;

		std::unordered_map<uint32, VkDescriptorSetLayout> m_DescriptorSetLayouts;
		std::unordered_map<uint32, std::vector<VkDescriptorPoolSize>> m_DescriptorPoolSizes;
		std::unordered_map<uint32, std::vector<VkDescriptorSetLayoutBinding>> m_DescriptorSetLayoutBindings;

		std::unordered_map<ShaderStage, VkShaderModule> m_ShaderModules;
	};

}