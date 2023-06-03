#include "FluxPCH.h"
#include "VulkanPipeline.h"

#include "Flux/Runtime/Renderer/Renderer.h"

#include "VulkanDevice.h"
#include "VulkanCommandBuffer.h"
#include "VulkanShader.h"
#include "VulkanFramebuffer.h"

namespace Flux {

	namespace Utils {

		static VkShaderStageFlagBits VulkanShaderStage(ShaderStage stage)
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

		static VkCompareOp VulkanCompareOp(CompareOp compareOp)
		{
			switch (compareOp)
			{
			case CompareOp::Never:          return VK_COMPARE_OP_NEVER;
			case CompareOp::Less:           return VK_COMPARE_OP_LESS;
			case CompareOp::Equal:          return VK_COMPARE_OP_EQUAL;
			case CompareOp::LessOrEqual:    return VK_COMPARE_OP_LESS_OR_EQUAL;
			case CompareOp::Greater:        return VK_COMPARE_OP_GREATER;
			case CompareOp::NotEqual:       return VK_COMPARE_OP_NOT_EQUAL;
			case CompareOp::GreaterOrEqual: return VK_COMPARE_OP_GREATER_OR_EQUAL;
			case CompareOp::Always:         return VK_COMPARE_OP_ALWAYS;
			}
			FLUX_VERIFY(false, "Unknown comparison function");
			return static_cast<VkCompareOp>(0);
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
		FLUX_VERIFY(createInfo.IsValid());

		Invalidate();
	}

	VulkanPipeline::~VulkanPipeline()
	{
		for (auto& [stage, storage] : m_PushConstantStorage)
			delete[] storage;

		FLUX_SUBMIT_RENDER_COMMAND_RELEASE([pipelineLayout = m_PipelineLayout, pipeline = m_Pipeline]()
		{
			VkDevice device = VulkanDevice::Get()->GetDevice();
			
			vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
			vkDestroyPipeline(device, pipeline, nullptr);
		});
	}

	void VulkanPipeline::Invalidate()
	{
		Ref<VulkanPipeline> instance = this;
		FLUX_SUBMIT_RENDER_COMMAND([instance]() mutable
		{
			instance->RT_Invalidate();
		});
	}

	void VulkanPipeline::RT_Invalidate()
	{
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

		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
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

		auto& inputLayout = shader->GetInputLayout();

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

	void VulkanPipeline::Bind(Ref<CommandBuffer> commandBuffer) const
	{
		Ref<const VulkanPipeline> instance = this;
		FLUX_SUBMIT_RENDER_COMMAND([instance, commandBuffer]()
		{
			instance->RT_Bind(commandBuffer);
		});
	}

	void VulkanPipeline::RT_Bind(Ref<CommandBuffer> commandBuffer) const
	{
		vkCmdBindPipeline(
			commandBuffer.As<VulkanCommandBuffer>()->GetActiveCommandBuffer(), 
			VK_PIPELINE_BIND_POINT_GRAPHICS, 
			m_Pipeline
		);
	}

	void VulkanPipeline::SetPushConstant(Ref<CommandBuffer> commandBuffer, ShaderStage stage, const void* data, uint32 size, uint32 offset)
	{
		if (m_PushConstantStorage.find(stage) == m_PushConstantStorage.end())
		{
			auto& pushConstants = m_CreateInfo.Shader->GetPushConstants();

			auto it = pushConstants.find(stage);
			if (it != pushConstants.end())
				m_PushConstantStorage[stage] = new uint8[it->second.Size];
			else
				FLUX_VERIFY(false);
		}

		uint8* destData = m_PushConstantStorage.at(stage);
		if (!destData)
		{
			FLUX_VERIFY(false);
			return;
		}

		// TODO: validate
		memcpy(destData, (uint8*)data + offset, size);

		Ref<const VulkanPipeline> instance = this;
		FLUX_SUBMIT_RENDER_COMMAND([instance, commandBuffer, stage, destData, size, offset]()
		{
			instance->RT_SetPushConstant(commandBuffer, stage, destData, size, offset);
		});
	}

	void VulkanPipeline::RT_SetPushConstant(Ref<CommandBuffer> commandBuffer, ShaderStage stage, const void* data, uint32 size, uint32 offset) const
	{
		vkCmdPushConstants(
			commandBuffer.As<VulkanCommandBuffer>()->GetActiveCommandBuffer(), 
			m_PipelineLayout, 
			Utils::VulkanShaderStage(stage), 
			offset, 
			size, 
			data
		);
	}

	void VulkanPipeline::DrawIndexed(Ref<CommandBuffer> commandBuffer, uint32 indexCount, uint32 startIndexLocation, uint32 baseVertexLocation) const
	{
		Ref<const VulkanPipeline> instance = this;
		FLUX_SUBMIT_RENDER_COMMAND([instance, commandBuffer, indexCount, startIndexLocation, baseVertexLocation]()
		{
			instance->RT_DrawIndexed(commandBuffer, indexCount, startIndexLocation, baseVertexLocation);
		});
	}

	void VulkanPipeline::RT_DrawIndexed(Ref<CommandBuffer> commandBuffer, uint32 indexCount, uint32 startIndexLocation, uint32 baseVertexLocation) const
	{
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