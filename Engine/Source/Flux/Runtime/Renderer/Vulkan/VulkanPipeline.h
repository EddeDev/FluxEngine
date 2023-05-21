#pragma once

#include "Flux/Runtime/Renderer/GraphicsPipeline.h"

#include "Vulkan.h"

namespace Flux {

	class VulkanPipeline : public GraphicsPipeline
	{
	public:
		VulkanPipeline(const GraphicsPipelineCreateInfo& createInfo);
		virtual ~VulkanPipeline();

		virtual void Invalidate() override;
		virtual void RT_Invalidate()  override;

		virtual void Bind(Ref<CommandBuffer> commandBuffer) const override;
		virtual void RT_Bind(Ref<CommandBuffer> commandBuffer) const override;

		virtual void DrawIndexed(Ref<CommandBuffer> commandBuffer, uint32 indexCount, uint32 startIndexLocation = 0, uint32 baseVertexLocation = 0) const override;
		virtual void RT_DrawIndexed(Ref<CommandBuffer> commandBuffer, uint32 indexCount, uint32 startIndexLocation = 0, uint32 baseVertexLocation = 0) const override;
	private:
		GraphicsPipelineCreateInfo m_CreateInfo;

		VkPipeline m_Pipeline = VK_NULL_HANDLE;
		VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
	};

}