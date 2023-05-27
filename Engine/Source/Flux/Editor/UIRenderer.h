#pragma once

#include "Flux/Runtime/Renderer/CommandBuffer.h"
#include "Flux/Runtime/Renderer/Framebuffer.h"
#include "Flux/Runtime/Renderer/GraphicsPipeline.h"
#include "Flux/Runtime/Renderer/Shader.h"
#include "Flux/Runtime/Renderer/VertexBuffer.h"
#include "Flux/Runtime/Renderer/IndexBuffer.h"

namespace Flux {

	class UIRenderer : public ReferenceCounted
	{
	public:
		UIRenderer();
		virtual ~UIRenderer();

		void BeginRendering();
		void EndRendering();

		void DrawRect(float minX, float minY, float maxX, float maxY, const glm::vec4& color = glm::vec4(1.0f));
	private:
		Ref<CommandBuffer> m_CommandBuffer;
		Ref<Framebuffer> m_SwapchainFramebuffer;
		Ref<GraphicsPipeline> m_Pipeline;

		struct Vertex
		{
			glm::vec3 Position;
			glm::vec4 Color;
		};

		inline static constexpr uint32 s_MaxPrimitives = 10000;
		inline static constexpr uint32 s_MaxVertices = s_MaxPrimitives * 4;
		inline static constexpr uint32 s_MaxIndices = s_MaxPrimitives * 6;

		Ref<Shader> m_Shader;
		std::vector<Ref<VertexBuffer>> m_VertexBuffer;
		std::vector<Vertex*> m_VertexStorage;
		Vertex* m_VertexPointer = nullptr;

		Ref<IndexBuffer> m_IndexBuffer;
		uint32 m_IndexCount = 0;
	};

}