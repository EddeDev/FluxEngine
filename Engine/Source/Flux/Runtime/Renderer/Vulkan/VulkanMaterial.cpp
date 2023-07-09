#include "FluxPCH.h"
#include "VulkanMaterial.h"

#include "Flux/Runtime/Core/Engine.h"
#include "Flux/Runtime/Renderer/Renderer.h"

#include "VulkanDevice.h"
#include "VulkanShader.h"
#include "VulkanPipeline.h"
#include "VulkanCommandBuffer.h"

namespace Flux {

	VulkanMaterial::VulkanMaterial(Ref<Shader> shader, const std::string& name)
		: m_Shader(shader), m_Name(name)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		CreatePushConstantStorage();
		CreateDescriptors();
	}

	VulkanMaterial::VulkanMaterial(Ref<RenderMaterial> other, const std::string& name)
		: m_Shader(other.As<VulkanMaterial>()->m_Shader), m_Name(name)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		CreatePushConstantStorage();
		CreateDescriptors();
	}

	VulkanMaterial::~VulkanMaterial()
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		std::lock_guard<std::mutex> lock(m_PushConstantMutex);

		delete[] m_PushConstantStorage;

		FLUX_SUBMIT_RENDER_COMMAND_RELEASE([descriptorPool = m_DescriptorPool]()
		{
			VkDevice device = VulkanDevice::Get()->GetDevice();

			vkDestroyDescriptorPool(device, descriptorPool, nullptr);
		});
	}

	void VulkanMaterial::CreatePushConstantStorage()
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		std::lock_guard<std::mutex> lock(m_PushConstantMutex);

		const auto& pushConstants = m_Shader->GetPushConstants();

		// TODO: copy members (by name) on shader reload

		auto it = pushConstants.find(ShaderStage::Fragment);
		if (it != pushConstants.end())
		{
			for (auto& [name, member] : it->second.Members)
				m_PushConstantMembers[name] = member;
		}

		uint32 size = 0;
		for (const auto& [name, member] : m_PushConstantMembers)
			size += member.Size;

		if (size > 0 && (size != m_PushConstantSize || !m_PushConstantStorage))
		{
			delete[] m_PushConstantStorage;

			m_PushConstantStorage = new uint8[size];
		}

		memset(m_PushConstantStorage, 0, size);

		m_PushConstantSize = size;
	}

	void VulkanMaterial::Bind(Ref<CommandBuffer> commandBuffer, Ref<GraphicsPipeline> pipeline)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		Ref<VulkanMaterial> instance = this;
		FLUX_SUBMIT_RENDER_COMMAND([instance, commandBuffer, pipeline]() mutable
		{
			instance->RT_Bind(commandBuffer, pipeline);
		});
	}

	void VulkanMaterial::RT_Bind(Ref<CommandBuffer> commandBuffer, Ref<GraphicsPipeline> pipeline)
	{
		FLUX_CHECK_IS_RENDER_THREAD();

		uint32 frameIndex = Renderer::RT_GetCurrentFrameIndex();

		auto it = m_DescriptorSets.find(frameIndex);
		if (it != m_DescriptorSets.end())
		{
			vkCmdBindDescriptorSets(
				commandBuffer.As<VulkanCommandBuffer>()->GetActiveCommandBuffer(),
				VK_PIPELINE_BIND_POINT_GRAPHICS,
				pipeline.As<VulkanPipeline>()->GetPipelineLayout(),
				0,
				1,
				&it->second,
				0,
				nullptr
			);
		}

		std::lock_guard<std::mutex> lock(m_PushConstantMutex);

		vkCmdPushConstants(
			commandBuffer.As<VulkanCommandBuffer>()->GetActiveCommandBuffer(),
			pipeline.As<VulkanPipeline>()->GetPipelineLayout(),
			VK_SHADER_STAGE_FRAGMENT_BIT,
			0,
			m_PushConstantSize,
			m_PushConstantStorage
		);
	}

	void VulkanMaterial::Set(std::string_view name, float value)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		Set<float>(name, value);
	}

	void VulkanMaterial::Set(std::string_view name, const glm::vec2& value)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		Set<glm::vec2>(name, value);
	}

	void VulkanMaterial::Set(std::string_view name, const glm::vec3& value)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		Set<glm::vec3>(name, value);
	}

	void VulkanMaterial::Set(std::string_view name, const glm::vec4& value)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		Set<glm::vec4>(name, value);
	}

	void VulkanMaterial::Set(std::string_view name, Ref<Image2D> image)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		const Descriptor* descriptor = m_Shader->GetDescriptor(name, DescriptorType::CombinedImageSampler);
		if (descriptor)
			m_Descriptors[descriptor->DescriptorSet][DescriptorType::CombinedImageSampler][descriptor->Binding] = image;
		else
			FLUX_WARNING("Could not find image descriptor: {0}", name);
	}

	void VulkanMaterial::CreateDescriptors()
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		Ref<VulkanMaterial> instance = this;
		FLUX_SUBMIT_RENDER_COMMAND([instance]() mutable
		{
			instance->RT_CreateDescriptors();
		});
	}

	void VulkanMaterial::RT_CreateDescriptors()
	{
		FLUX_CHECK_IS_RENDER_THREAD();

		VkDevice device = VulkanDevice::Get()->GetDevice();

		Ref<VulkanShader> shader = m_Shader.As<VulkanShader>();

		std::unordered_map<VkDescriptorType, uint32> descriptorPoolSizes;

		for (const auto& [set, descriptorSet] : shader->GetDescriptorSets())
		{
			for (const auto& [descriptorType, descriptors] : descriptorSet)
			{
				for (const auto& [binding, descriptor] : descriptors)
				{
					if (descriptor.Stage != ShaderStage::Fragment)
						continue;

					if (descriptor.Type == DescriptorType::UniformBuffer || descriptor.Type == DescriptorType::StorageBuffer)
						continue;
					
					descriptorPoolSizes[Utils::VulkanDescriptorType(descriptorType)] += descriptor.Count;
				}
			}
		}

		if (descriptorPoolSizes.empty())
		{
			FLUX_WARNING("[{0}]: Empty descriptor pool!", m_Name);
			return;
		}

		std::vector<VkDescriptorPoolSize> typeCounts;
		for (auto& [type, count] : descriptorPoolSizes)
			typeCounts.push_back({ type, count });

		VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
		descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		descriptorPoolCreateInfo.maxSets = static_cast<uint32>(descriptorPoolSizes.size()) * static_cast<uint32>(m_Descriptors.size());
		descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32>(typeCounts.size());
		descriptorPoolCreateInfo.pPoolSizes = typeCounts.data();

		if (m_DescriptorPool)
			vkDestroyDescriptorPool(device, m_DescriptorPool, nullptr);

		VK_CHECK(vkCreateDescriptorPool(device, &descriptorPoolCreateInfo, nullptr, &m_DescriptorPool));

		m_DescriptorSets.clear();

		for (uint32 frameIndex = 0; frameIndex < Renderer::GetFramesInFlight(); frameIndex++)
		{
#if 0
			for (const auto& [set, descriptorSet] : shader->GetDescriptorSets())
			{
				for (const auto& [descriptorType, descriptors] : descriptorSet)
				{
					for (const auto& [binding, descriptor] : descriptors)
					{
						bool hasDescriptor = false;

						auto frameIt = m_Descriptors.find(frameIndex);
						if (frameIt != m_Descriptors.end())
						{
							auto setIt = frameIt->second.find(set);
							if (setIt != frameIt->second.end())
							{
								auto typeIt = setIt->second.find(descriptorType);
								if (typeIt != setIt->second.end())
								{
									auto bindingIt = typeIt->second.find(binding);
									if (bindingIt != typeIt->second.end())
									{
										if (bindingIt->second)
											hasDescriptor = true;
									}
								}
							}
						}

						if (!hasDescriptor)
						{
							FLUX_VERIFY(false, "{0} '{1}' ({2}.{3}) is not set for frame {4}!",
								Utils::DescriptorTypeToString(descriptorType),
								descriptor.Name,
								descriptor.DescriptorSet, descriptor.Binding,
								frameIndex
							);
						}
					}
				}
			}
#endif
		}
	}

}