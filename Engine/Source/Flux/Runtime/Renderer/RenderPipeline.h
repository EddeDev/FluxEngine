#pragma once

#include "CommandBuffer.h"
#include "GraphicsPipeline.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

namespace Flux {

	class RenderPipeline : public ReferenceCounted
	{
	public:
		virtual ~RenderPipeline() {}

		virtual void BeginRendering() = 0;
		virtual void EndRendering() = 0;

		virtual void BeginRendering2D() = 0;
		virtual void EndRendering2D() = 0;

		virtual void DrawQuad(const glm::mat4& transform, const glm::vec4& color = glm::vec4(1.0f)) = 0;
		virtual void DrawQuad(const glm::vec3& position, float rotation, const glm::vec2& scale, const glm::vec4& color = glm::vec4(1.0f)) = 0;
		virtual void DrawQuad(const glm::vec3& position, const glm::vec2& scale, const glm::vec4& color = glm::vec4(1.0f)) = 0;

		virtual void SetViewportSize(uint32 width, uint32 height) = 0;
	};

	class ForwardRenderPipeline : public RenderPipeline
	{
	public:
		ForwardRenderPipeline();
		virtual ~ForwardRenderPipeline();

		virtual void BeginRendering() override;
		virtual void EndRendering() override;

		virtual void BeginRendering2D() override;
		virtual void EndRendering2D() override;

		virtual void DrawQuad(const glm::mat4& transform, const glm::vec4& color = glm::vec4(1.0f)) override;
		virtual void DrawQuad(const glm::vec3& position, float rotation, const glm::vec2& scale, const glm::vec4& color = glm::vec4(1.0f)) override;
		virtual void DrawQuad(const glm::vec3& position, const glm::vec2& scale, const glm::vec4& color = glm::vec4(1.0f)) override;
		
		virtual void SetViewportSize(uint32 width, uint32 height) override;
	private:
		uint32 m_ViewportWidth = 0;
		uint32 m_ViewportHeight = 0;

		Ref<CommandBuffer> m_CommandBuffer;
		Ref<Framebuffer> m_Framebuffer;

		struct QuadVertex
		{
			glm::vec3 Position;
			glm::vec4 Color;
		};

		inline static constexpr uint32 s_MaxQuads = 20000;
		inline static constexpr uint32 s_MaxQuadVertices = s_MaxQuads * 4;
		inline static constexpr uint32 s_MaxQuadIndices = s_MaxQuads * 6;

		Ref<GraphicsPipeline> m_QuadPipeline;
		Ref<Shader> m_QuadShader;
		std::vector<Ref<VertexBuffer>> m_QuadVertexBuffer;
		std::vector<QuadVertex*> m_QuadVertexStorage;
		QuadVertex* m_QuadVertexPointer = nullptr;

		Ref<IndexBuffer> m_QuadIndexBuffer;
		uint32 m_QuadIndexCount = 0;
	};

}