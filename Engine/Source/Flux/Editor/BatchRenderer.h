#pragma once

#include "Flux/Runtime/Renderer/CommandBuffer.h"
#include "Flux/Runtime/Renderer/Framebuffer.h"
#include "Flux/Runtime/Renderer/GraphicsPipeline.h"
#include "Flux/Runtime/Renderer/Shader.h"
#include "Flux/Runtime/Renderer/VertexBuffer.h"
#include "Flux/Runtime/Renderer/IndexBuffer.h"

namespace Flux {

	class BatchRenderer : public ReferenceCounted
	{
	public:
		BatchRenderer();
		virtual ~BatchRenderer();

		void BeginRendering();
		void EndRendering();

		void DrawQuad(const glm::mat4& transform, const glm::vec4& color = glm::vec4(1.0f));
		void DrawQuad(const glm::vec3& position, float rotation, const glm::vec2& scale, const glm::vec4& color = glm::vec4(1.0f));
		void DrawQuad(const glm::vec3& position, const glm::vec2& scale, const glm::vec4& color = glm::vec4(1.0f));

		void DrawRect(float minX, float minY, float maxX, float maxY, const glm::vec4& color = glm::vec4(1.0f));
	private:
		Ref<CommandBuffer> m_CommandBuffer;
		Ref<Framebuffer> m_SwapchainFramebuffer;

		struct QuadVertex
		{
			glm::vec3 Position;
			glm::vec4 Color;
		};

		inline static constexpr uint32 s_MaxQuads = 10000;
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