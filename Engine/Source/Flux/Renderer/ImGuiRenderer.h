#pragma once

#include "Shader.h"
#include "GraphicsPipeline.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

#include <imgui_internal.h>

namespace Flux {

	class ImGuiRenderer : public ReferenceCounted
	{
	public:
		ImGuiRenderer();
		virtual ~ImGuiRenderer();

		void BeginFrame();
		void EndFrame();
	private:
		ImGuiContext* m_Context = nullptr;

		Ref<Shader> m_Shader;
		Ref<GraphicsPipeline> m_Pipeline;
		Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer> m_IndexBuffer;

		uint32 m_FontTextureID = 0;
	};

}