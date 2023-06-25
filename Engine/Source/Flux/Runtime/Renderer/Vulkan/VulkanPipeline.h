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
		virtual void RT_Invalidate() override;

		virtual bool SetUniformBuffer(std::string_view name, Ref<UniformBuffer> uniformBuffer, uint32 frameIndex) override;
		virtual Ref<UniformBuffer> GetUniformBuffer(std::string_view name, uint32 frameIndex) const override;

		virtual void Bake() override;
		virtual void RT_Bake() override;

		virtual void Bind(Ref<CommandBuffer> commandBuffer) const override;
		virtual void RT_Bind(Ref<CommandBuffer> commandBuffer) const override;

		virtual void DrawIndexed(Ref<CommandBuffer> commandBuffer, uint32 indexCount, uint32 startIndexLocation = 0, uint32 baseVertexLocation = 0) const override;
		virtual void RT_DrawIndexed(Ref<CommandBuffer> commandBuffer, uint32 indexCount, uint32 startIndexLocation = 0, uint32 baseVertexLocation = 0) const override;
		
		virtual void RT_SetPushConstant(Ref<CommandBuffer> commandBuffer, ShaderStage stage, const void* data, uint32 size, uint32 offset = 0) const override;
	
		virtual void BindDescriptorSets(Ref<CommandBuffer> commandBuffer) const override;
		virtual void RT_BindDescriptorSets(Ref<CommandBuffer> commandBuffer) const override;

		virtual bool IsValid() const { return m_CreateInfo.IsValid(); }

		virtual Ref<Shader> GetShader() const override { return m_CreateInfo.Shader; }
		virtual Ref<Framebuffer> GetFramebuffer() const override { return m_CreateInfo.Framebuffer; }
	private:
		const Descriptor* GetDescriptor(std::string_view name, DescriptorType type) const;
	private:
		GraphicsPipelineCreateInfo m_CreateInfo;

		VkPipeline m_Pipeline = VK_NULL_HANDLE;
		VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
		VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;

		// frame -> set -> type -> binding
		std::map<uint32, std::map<uint32, std::map<DescriptorType, std::map<uint32, Ref<ReferenceCounted>>>>> m_Descriptors;
		std::map<uint32, std::vector<VkDescriptorSet>> m_DescriptorSets;
	};

}