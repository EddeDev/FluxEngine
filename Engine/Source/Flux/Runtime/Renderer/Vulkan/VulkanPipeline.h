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
		
		virtual void RT_SetPushConstant(Ref<CommandBuffer> commandBuffer, ShaderStage stage, const void* data, uint32 size, uint32 offset = 0) const override;
	
		virtual bool IsValid() const { return m_CreateInfo.Shader && m_CreateInfo.Framebuffer; }

		virtual Ref<Shader> GetShader() const override { return m_CreateInfo.Shader; }
		virtual Ref<Framebuffer> GetFramebuffer() const override { return m_CreateInfo.Framebuffer; }
	private:
		GraphicsPipelineCreateInfo m_CreateInfo;

		VkPipeline m_Pipeline = VK_NULL_HANDLE;
		VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
	};

}