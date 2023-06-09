#pragma once

#include "Shader.h"
#include "Framebuffer.h"
#include "UniformBuffer.h"
#include "CompareOp.h"

namespace Flux {

	enum class PrimitiveTopology : uint8
	{
		Triangles = 0,
		Lines,
		Points
	};

	struct GraphicsPipelineCreateInfo
	{
		Ref<Shader> Shader;
		Ref<Framebuffer> Framebuffer;
		PrimitiveTopology Topology = PrimitiveTopology::Triangles;
		CompareOp DepthCompareOp = CompareOp::Less;
		bool DepthTest = true;
		bool DepthWrite = true;
		bool FrontCounterClockwise = true;
		bool BackfaceCulling = false;
		bool Wireframe = false;
		float LineWidth = 1.0f;

		std::string DebugLabel = "Pipeline";

		bool IsValid() const
		{
			return Shader && Framebuffer;
		}
	};

	class GraphicsPipeline : public ReferenceCounted
	{
	public:
		virtual ~GraphicsPipeline() {}

		virtual void Invalidate() = 0;
		virtual void RT_Invalidate() = 0;

		virtual bool SetUniformBuffer(std::string_view name, Ref<UniformBuffer> uniformBuffer, uint32 frameIndex) = 0;
		virtual Ref<UniformBuffer> GetUniformBuffer(std::string_view name, uint32 frameIndex) = 0;

		virtual void Bake() = 0;
		virtual void RT_Bake() = 0;

		virtual void Bind(Ref<CommandBuffer> commandBuffer) const = 0;
		virtual void RT_Bind(Ref<CommandBuffer> commandBuffer) const = 0;

		virtual void DrawIndexed(Ref<CommandBuffer> commandBuffer, uint32 indexCount, uint32 startIndexLocation = 0, uint32 baseVertexLocation = 0) const = 0;
		virtual void RT_DrawIndexed(Ref<CommandBuffer> commandBuffer, uint32 indexCount, uint32 startIndexLocation = 0, uint32 baseVertexLocation = 0) const = 0;

		virtual void RT_SetPushConstant(Ref<CommandBuffer> commandBuffer, ShaderStage stage, const void* data, uint32 size, uint32 offset = 0) const = 0;

		virtual void BindDescriptorSets(Ref<CommandBuffer> commandBuffer) const = 0;
		virtual void RT_BindDescriptorSets(Ref<CommandBuffer> commandBuffer) const = 0;

		virtual Ref<Shader> GetShader() const = 0;
		virtual Ref<Framebuffer> GetFramebuffer() const = 0;

		static Ref<GraphicsPipeline> Create(const GraphicsPipelineCreateInfo& createInfo);
	};

}