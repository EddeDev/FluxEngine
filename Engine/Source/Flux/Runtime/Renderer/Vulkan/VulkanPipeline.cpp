#include "FluxPCH.h"
#include "VulkanPipeline.h"

#include "Flux/Runtime/Core/Engine.h"
#include "Flux/Runtime/Renderer/Renderer.h"

#include "VulkanDevice.h"
#include "VulkanCommandBuffer.h"
#include "VulkanShader.h"
#include "VulkanFramebuffer.h"
#include "VulkanUniformBuffer.h"
#include "VulkanCompareOp.h"

namespace Flux {

	namespace Utils {

		static VkPrimitiveTopology VulkanPrimitiveTopology(PrimitiveTopology topology)
		{
			switch (topology)
			{
			case PrimitiveTopology::Triangles: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			case PrimitiveTopology::Lines:     return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
			case PrimitiveTopology::Points:    return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
			}
			FLUX_VERIFY(false, "Unknown primitive topology");
			return static_cast<VkPrimitiveTopology>(0);
		}

		static VkFormat VulkanShaderDataType(ShaderDataType type)
		{
			switch (type)
			{
			case ShaderDataType::Float:  return VK_FORMAT_R32_SFLOAT;
			case ShaderDataType::Float2: return VK_FORMAT_R32G32_SFLOAT;
			case ShaderDataType::Float3: return VK_FORMAT_R32G32B32_SFLOAT;
			case ShaderDataType::Float4: return VK_FORMAT_R32G32B32A32_SFLOAT;
			case ShaderDataType::Int:    return VK_FORMAT_R32_SINT;
			case ShaderDataType::Int2:   return VK_FORMAT_R32G32_SINT;
			case ShaderDataType::Int3:   return VK_FORMAT_R32G32B32_SINT;
			case ShaderDataType::Int4:   return VK_FORMAT_R32G32B32A32_SINT;
			case ShaderDataType::UInt:   return VK_FORMAT_R32_UINT;
			case ShaderDataType::UInt2:  return VK_FORMAT_R32G32_UINT;
			case ShaderDataType::UInt3:  return VK_FORMAT_R32G32B32_UINT;
			case ShaderDataType::UInt4:  return VK_FORMAT_R32G32B32A32_UINT;
			}
			FLUX_VERIFY(false, "Unknown shader data type");
			return VK_FORMAT_UNDEFINED;
		}

	}

	VulkanPipeline::VulkanPipeline(const GraphicsPipelineCreateInfo& createInfo)
		: m_CreateInfo(createInfo)
	{
		FLUX_CHECK_IS_MAIN_THREAD();
		FLUX_VERIFY(createInfo.IsValid());

		Invalidate();
	}

	VulkanPipeline::~VulkanPipeline()
	{
		FLUX_CHECK_IS_MAIN_THREAD();
		
		FLUX_SUBMIT_RENDER_COMMAND_RELEASE([pipelineLayout = m_PipelineLayout, pipeline = m_Pipeline, descriptorPool = m_DescriptorPool]()
		{
			VkDevice device = VulkanDevice::Get()->GetDevice();
			
			vkDestroyDescriptorPool(device, descriptorPool, nullptr);
			vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
			vkDestroyPipeline(device, pipeline, nullptr);
		});
	}

	void VulkanPipeline::Invalidate()
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		Ref<VulkanPipeline> instance = this;
		FLUX_SUBMIT_RENDER_COMMAND([instance]() mutable
		{
			instance->RT_Invalidate();
		});
	}

	void VulkanPipeline::RT_Invalidate()
	{
		FLUX_CHECK_IS_RENDER_THREAD();

		VkDevice device = VulkanDevice::Get()->GetDevice();
		VkPipelineCache pipelineCache = VulkanDevice::Get()->GetPipelineCache();

		Ref<VulkanFramebuffer> framebuffer = m_CreateInfo.Framebuffer.As<VulkanFramebuffer>();

		// TODO: Release function?
		if (m_PipelineLayout)
			vkDestroyPipelineLayout(device, m_PipelineLayout, nullptr);
		if (m_Pipeline)
			vkDestroyPipeline(device, m_Pipeline, nullptr);

		Ref<VulkanShader> shader = m_CreateInfo.Shader.As<VulkanShader>();

		std::vector<VkPushConstantRange> pushConstantRanges;
		for (auto& [stage, pushConstant] : shader->GetPushConstants())
		{
			auto& pushConstantRange = pushConstantRanges.emplace_back();
			pushConstantRange.stageFlags = Utils::VulkanShaderStage(stage);
			pushConstantRange.size = pushConstant.Size;
			pushConstantRange.offset = pushConstant.Offset;
		}

		int32 descriptorSetLayoutCount = 0;
		for (auto& [set, layout] : shader->GetDescriptorSetLayouts())
		{
			if ((int32)set > (int32)descriptorSetLayoutCount - 1)
				descriptorSetLayoutCount = set + 1;
		}

		std::vector<VkDescriptorSetLayout> descriptorSetLayouts(descriptorSetLayoutCount);
		for (auto& [set, layout] : shader->GetDescriptorSetLayouts())
			descriptorSetLayouts[set] = layout;

		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32>(descriptorSetLayouts.size());
		pipelineLayoutCreateInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineLayoutCreateInfo.pushConstantRangeCount = static_cast<uint32>(pushConstantRanges.size());
		pipelineLayoutCreateInfo.pPushConstantRanges = pushConstantRanges.data();
		VK_CHECK(vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &m_PipelineLayout));

		std::vector<VkPipelineShaderStageCreateInfo> stageCreateInfos;
		for (auto& [stage, shaderModule] : shader->GetShaderModules())
		{
			auto& createInfo = stageCreateInfos.emplace_back();
			createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			createInfo.stage = Utils::VulkanShaderStage(stage);
			createInfo.module = shaderModule;
			createInfo.pName = "main";
		}

		auto& inputLayout = shader->GetVertexInputLayout();

		std::vector<VkVertexInputBindingDescription> vertexBindingDescriptions;
		vertexBindingDescriptions.emplace_back() = {
			.binding = 0,
			.stride = inputLayout.Stride,
			.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
		};

		std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions;
		for (const auto& attribute : inputLayout)
		{
			auto& description = vertexAttributeDescriptions.emplace_back();
			description.location = attribute.Location;
			description.binding = attribute.Binding;
			description.format = Utils::VulkanShaderDataType(attribute.Type);
			description.offset = attribute.Offset;
		}

		VkPipelineVertexInputStateCreateInfo vertexInputState = {};
		vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputState.vertexBindingDescriptionCount = static_cast<uint32>(vertexBindingDescriptions.size());
		vertexInputState.pVertexBindingDescriptions = vertexBindingDescriptions.data();
		vertexInputState.vertexAttributeDescriptionCount = static_cast<uint32>(vertexAttributeDescriptions.size());
		vertexInputState.pVertexAttributeDescriptions = vertexAttributeDescriptions.data();

		VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = {};
		inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssemblyState.topology = Utils::VulkanPrimitiveTopology(m_CreateInfo.Topology);
		inputAssemblyState.primitiveRestartEnable = VK_FALSE;

		VkViewport viewport;
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)m_CreateInfo.Framebuffer->GetWidth();
		viewport.height = (float)m_CreateInfo.Framebuffer->GetHeight();
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor;
		scissor.offset = { 0, 0 };
		scissor.extent = { m_CreateInfo.Framebuffer->GetWidth(), m_CreateInfo.Framebuffer->GetHeight() };

		VkPipelineViewportStateCreateInfo viewportState = {};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissor;

		VkPipelineRasterizationStateCreateInfo rasterizationState = {};
		rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizationState.depthClampEnable = VK_FALSE;
		rasterizationState.rasterizerDiscardEnable = VK_FALSE;
		rasterizationState.polygonMode = m_CreateInfo.Wireframe ? VK_POLYGON_MODE_LINE : VK_POLYGON_MODE_FILL;
		rasterizationState.cullMode = m_CreateInfo.BackfaceCulling ? VK_CULL_MODE_BACK_BIT : VK_CULL_MODE_NONE;
		rasterizationState.frontFace = m_CreateInfo.FrontCounterClockwise ? VK_FRONT_FACE_COUNTER_CLOCKWISE : VK_FRONT_FACE_CLOCKWISE;
		rasterizationState.depthBiasClamp = VK_FALSE;
		rasterizationState.depthBiasSlopeFactor = 0.0f;
		rasterizationState.lineWidth = m_CreateInfo.LineWidth;

		VkPipelineMultisampleStateCreateInfo multisampleState = {};
		multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampleState.minSampleShading = 1.0f;

		VkPipelineDepthStencilStateCreateInfo depthStencilState = {};
		depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencilState.depthTestEnable = m_CreateInfo.DepthTest;
		depthStencilState.depthWriteEnable = m_CreateInfo.DepthWrite;
		depthStencilState.depthCompareOp = Utils::VulkanCompareOp(m_CreateInfo.DepthCompareOp);
		depthStencilState.depthBoundsTestEnable = VK_FALSE;
		depthStencilState.stencilTestEnable = VK_FALSE;
		// depthStencilState.front
		// depthStencilState.back
		depthStencilState.minDepthBounds = 0.0f;
		depthStencilState.maxDepthBounds = 1.0f;

		VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;

		VkPipelineColorBlendStateCreateInfo colorBlendState = {};
		colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendState.logicOpEnable = VK_FALSE;
		colorBlendState.logicOp = VK_LOGIC_OP_COPY;
		colorBlendState.attachmentCount = 1;
		colorBlendState.pAttachments = &colorBlendAttachment;

		const std::vector<VkDynamicState> dynamicStates = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

		VkPipelineDynamicStateCreateInfo dynamicState = {};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = static_cast<uint32>(dynamicStates.size());
		dynamicState.pDynamicStates = dynamicStates.data();

		VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
		pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineCreateInfo.stageCount = static_cast<uint32>(stageCreateInfos.size());
		pipelineCreateInfo.pStages = stageCreateInfos.data();
		pipelineCreateInfo.pVertexInputState = &vertexInputState;
		pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
		pipelineCreateInfo.pViewportState = &viewportState;
		pipelineCreateInfo.pRasterizationState = &rasterizationState;
		pipelineCreateInfo.pMultisampleState = &multisampleState;
		pipelineCreateInfo.pDepthStencilState = &depthStencilState;
		pipelineCreateInfo.pColorBlendState = &colorBlendState;
		pipelineCreateInfo.pDynamicState = &dynamicState;
		pipelineCreateInfo.layout = m_PipelineLayout;
		pipelineCreateInfo.basePipelineIndex = -1;
		pipelineCreateInfo.renderPass = framebuffer->GetRenderPass();
		pipelineCreateInfo.subpass = 0;

		VK_CHECK(vkCreateGraphicsPipelines(device, pipelineCache, 1, &pipelineCreateInfo, nullptr, &m_Pipeline));
	}

	bool VulkanPipeline::SetUniformBuffer(std::string_view name, Ref<UniformBuffer> uniformBuffer)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		const Descriptor* descriptor = GetDescriptor(name, DescriptorType::UniformBuffer);
		if (descriptor)
		{
#if 0
			FLUX_INFO("[{0}]: Setting Uniform Buffer descriptor '{1}' in {2} shader '{3}' ({4}.{5})", 
				m_CreateInfo.DebugLabel, name, 
				descriptor->Stage == ShaderStage::Vertex ? "vertex" : 
				descriptor->Stage == ShaderStage::Fragment ? "fragment" :
				descriptor->Stage == ShaderStage::Compute ? "compute" : "<invalid>",
				m_CreateInfo.Shader->GetPath().filename().string(),
				descriptor->DescriptorSet, descriptor->Binding
			);
#endif
			
			m_Descriptors[descriptor->DescriptorSet][DescriptorType::UniformBuffer][descriptor->Binding] = uniformBuffer;
		}			
		else
		{
			FLUX_WARNING("Could not find uniform buffer descriptor: {0}", name);
		}
		return true;
	}

	Ref<UniformBuffer> VulkanPipeline::GetUniformBuffer(std::string_view name) const
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		const Descriptor* descriptor = GetDescriptor(name, DescriptorType::UniformBuffer);
		if (descriptor)
		{
			auto setIt = m_Descriptors.find(descriptor->DescriptorSet);
			if (setIt != m_Descriptors.end())
			{
				auto typeIt = setIt->second.find(descriptor->Type);
				if (typeIt != setIt->second.end())
				{
					auto it = typeIt->second.find(descriptor->Binding);
					if (it != typeIt->second.end())
						return it->second;
				}
			}
		}
		return nullptr;
	}

	// TODO: cache this
	const Descriptor* VulkanPipeline::GetDescriptor(std::string_view name, DescriptorType type) const
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		for (const auto& [set, descriptorSet] : m_CreateInfo.Shader->GetDescriptorSets())
		{
			for (const auto& [descriptorType, descriptors] : descriptorSet)
			{
				if (descriptorType != type)
					continue;

				for (const auto& [binding, descriptor] : descriptors)
				{
					if (descriptor.Name == name)
						return &descriptor;
				}
			}
		}
		return nullptr;
	}

	void VulkanPipeline::Bake()
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		Ref<VulkanPipeline> instance = this;
		FLUX_SUBMIT_RENDER_COMMAND([instance]() mutable
		{
			instance->RT_Bake();
		});
	}

	void VulkanPipeline::RT_Bake()
	{
		FLUX_CHECK_IS_RENDER_THREAD();

		VkDevice device = VulkanDevice::Get()->GetDevice();

		Ref<VulkanShader> shader = m_CreateInfo.Shader.As<VulkanShader>();

		for (const auto& [set, descriptorSet] : shader->GetDescriptorSets())
		{
			for (const auto& [descriptorType, descriptors] : descriptorSet)
			{
				for (const auto& [binding, descriptor] : descriptors)
				{
					bool hasDescriptor = false;

					auto setIt = m_Descriptors.find(set);
					if (setIt != m_Descriptors.end())
					{
						auto typeIt = setIt->second.find(descriptorType);
						if (typeIt != setIt->second.end())
						{
							auto it = typeIt->second.find(binding);
							if (it != typeIt->second.end())
							{
								if (it->second)
									hasDescriptor = true;
							}
						}
					}

					if (!hasDescriptor)
					{
						FLUX_VERIFY(false, "{0} '{1}' ({2}.{3}) is not set!", 
							Utils::DescriptorTypeToString(descriptorType),
							descriptor.Name,
							descriptor.DescriptorSet, descriptor.Binding
						);
					}
				}
			}
		}
	
		std::unordered_map<VkDescriptorType, uint32> descriptorPoolSizes;
		for (auto& [set, poolSizes] : shader->GetDescriptorPoolSizes())
		{
			for (auto& poolSize : poolSizes)
				descriptorPoolSizes[poolSize.type] += poolSize.descriptorCount;
		}

		if (descriptorPoolSizes.empty())
		{
			FLUX_WARNING("[{0}]: Empty descriptor pool!", m_CreateInfo.DebugLabel);
			return;
		}

		std::vector<VkDescriptorPoolSize> typeCounts;
		for (auto& [type, count] : descriptorPoolSizes)
			typeCounts.push_back({ type, count });

		VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
		descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		descriptorPoolCreateInfo.maxSets = static_cast<uint32>(descriptorPoolSizes.size());
		descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32>(typeCounts.size());
		descriptorPoolCreateInfo.pPoolSizes = typeCounts.data();

		if (m_DescriptorPool)
			vkDestroyDescriptorPool(device, m_DescriptorPool, nullptr);

		VK_CHECK(vkCreateDescriptorPool(device, &descriptorPoolCreateInfo, nullptr, &m_DescriptorPool));

		m_DesciptorSets.resize(m_Descriptors.size());

		std::vector<VkWriteDescriptorSet> descriptorWrites;
		for (const auto& [set, descriptorSets] : m_Descriptors)
		{
			VkDescriptorSetLayout descriptorSetLayout = shader->GetDescriptorSetLayout(set);

			VkDescriptorSetAllocateInfo descriptorSetAllocateInfo = {};
			descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			descriptorSetAllocateInfo.descriptorPool = m_DescriptorPool;
			descriptorSetAllocateInfo.descriptorSetCount = 1;
			descriptorSetAllocateInfo.pSetLayouts = &descriptorSetLayout;

			VK_CHECK(vkAllocateDescriptorSets(device, &descriptorSetAllocateInfo, &m_DesciptorSets[set]));

			for (const auto& [descriptorType, descriptors] : descriptorSets)
			{
				for (const auto& [binding, descriptor] : descriptors)
				{
					if (!descriptor)
					{
						FLUX_VERIFY(false);
						continue;
					}

					auto& descriptorWrite = descriptorWrites.emplace_back();
					descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					descriptorWrite.dstSet = m_DesciptorSets.at(set);
					descriptorWrite.dstBinding = binding;
					descriptorWrite.dstArrayElement = 0;
					descriptorWrite.descriptorCount = 1;

					switch (descriptorType)
					{
					case DescriptorType::UniformBuffer:
					{
						Ref<VulkanUniformBuffer> uniformBuffer = descriptor.As<VulkanUniformBuffer>();
						FLUX_VERIFY(binding == uniformBuffer->GetBinding());

						VkDescriptorBufferInfo descriptorBufferInfo = {};
						descriptorBufferInfo.buffer = uniformBuffer->GetBuffer();
						descriptorBufferInfo.offset = 0;
						descriptorBufferInfo.range = uniformBuffer->GetSize();

						descriptorWrite.pBufferInfo = &descriptorBufferInfo;
						descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
						break;
					}
					}
				}
			}
		}

		vkUpdateDescriptorSets(device, static_cast<uint32>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}

	void VulkanPipeline::Bind(Ref<CommandBuffer> commandBuffer) const
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		Ref<const VulkanPipeline> instance = this;
		FLUX_SUBMIT_RENDER_COMMAND([instance, commandBuffer]()
		{
			instance->RT_Bind(commandBuffer);
		});
	}

	void VulkanPipeline::RT_Bind(Ref<CommandBuffer> commandBuffer) const
	{
		FLUX_CHECK_IS_RENDER_THREAD();

		vkCmdBindPipeline(
			commandBuffer.As<VulkanCommandBuffer>()->GetActiveCommandBuffer(), 
			VK_PIPELINE_BIND_POINT_GRAPHICS, 
			m_Pipeline
		);
	}

	void VulkanPipeline::RT_SetPushConstant(Ref<CommandBuffer> commandBuffer, ShaderStage stage, const void* data, uint32 size, uint32 offset) const
	{
		FLUX_CHECK_IS_RENDER_THREAD();

		vkCmdPushConstants(
			commandBuffer.As<VulkanCommandBuffer>()->GetActiveCommandBuffer(), 
			m_PipelineLayout, 
			Utils::VulkanShaderStage(stage), 
			offset, 
			size, 
			data
		);
	}

	void VulkanPipeline::BindDescriptorSets(Ref<CommandBuffer> commandBuffer) const
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		Ref<const VulkanPipeline> instance = this;
		FLUX_SUBMIT_RENDER_COMMAND([instance, commandBuffer]()
		{
			instance->RT_BindDescriptorSets(commandBuffer);
		});
	}

	void VulkanPipeline::RT_BindDescriptorSets(Ref<CommandBuffer> commandBuffer) const
	{
		FLUX_CHECK_IS_RENDER_THREAD();

		if (m_DesciptorSets.empty())
			return;

		vkCmdBindDescriptorSets(
			commandBuffer.As<VulkanCommandBuffer>()->GetActiveCommandBuffer(),
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_PipelineLayout,
			0,
			static_cast<uint32>(m_DesciptorSets.size()),
			m_DesciptorSets.data(),
			0,
			nullptr
		);
	}

	void VulkanPipeline::DrawIndexed(Ref<CommandBuffer> commandBuffer, uint32 indexCount, uint32 startIndexLocation, uint32 baseVertexLocation) const
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		Ref<const VulkanPipeline> instance = this;
		FLUX_SUBMIT_RENDER_COMMAND([instance, commandBuffer, indexCount, startIndexLocation, baseVertexLocation]()
		{
			instance->RT_DrawIndexed(commandBuffer, indexCount, startIndexLocation, baseVertexLocation);
		});
	}

	void VulkanPipeline::RT_DrawIndexed(Ref<CommandBuffer> commandBuffer, uint32 indexCount, uint32 startIndexLocation, uint32 baseVertexLocation) const
	{
		FLUX_CHECK_IS_RENDER_THREAD();

		vkCmdDrawIndexed(
			commandBuffer.As<VulkanCommandBuffer>()->GetActiveCommandBuffer(),
			indexCount,
			1,
			startIndexLocation * sizeof(uint32), /* TODO: IndexBufferDataType */
			baseVertexLocation,
			0
		);
	}

}