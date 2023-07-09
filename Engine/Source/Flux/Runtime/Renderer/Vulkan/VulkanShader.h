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

		virtual const VertexInputLayout& GetVertexInputLayout() const override { return m_VertexInputLayout; }
		virtual const PushConstantMap& GetPushConstants() const override { return m_PushConstants; }
		virtual const DescriptorSetMap& GetDescriptorSets() const override { return m_DescriptorSets; }

		virtual const Descriptor* GetDescriptor(std::string_view name, DescriptorType type) const override;

		virtual const std::filesystem::path& GetPath() const override { return m_Path; }

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

	namespace Utils {

		inline static VkShaderStageFlagBits VulkanShaderStage(ShaderStage stage)
		{
			switch (stage)
			{
			case ShaderStage::Vertex:   return VK_SHADER_STAGE_VERTEX_BIT;
			case ShaderStage::Fragment: return VK_SHADER_STAGE_FRAGMENT_BIT;
			case ShaderStage::Compute:  return VK_SHADER_STAGE_COMPUTE_BIT;
			}
			FLUX_VERIFY(false, "Unknown shader stage");
			return static_cast<VkShaderStageFlagBits>(0);
		}

		inline static VkDescriptorType VulkanDescriptorType(DescriptorType type)
		{
			switch (type)
			{
			case DescriptorType::UniformBuffer:        return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			case DescriptorType::StorageBuffer:        return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			case DescriptorType::CombinedImageSampler: return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			case DescriptorType::SampledImage:         return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			case DescriptorType::StorageImage:         return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
			}
			FLUX_VERIFY(false, "Unknown descriptor type");
			return static_cast<VkDescriptorType>(0);
		}

	}

}