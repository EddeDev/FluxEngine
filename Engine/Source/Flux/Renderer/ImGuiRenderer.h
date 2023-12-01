#pragma once

#include "Shader.h"
#include "GraphicsPipeline.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Texture.h"

#include <imgui_internal.h>

namespace Flux {

	class ImGuiRenderer : public ReferenceCounted
	{
	public:
		ImGuiRenderer();
		virtual ~ImGuiRenderer();

		void NewFrame();
		void Render();

		void Image(Ref<Texture2D> texture, const ImVec2& size);
	private:
		ImGuiContext* m_Context = nullptr;

		Ref<Shader> m_Shader;
		Ref<GraphicsPipeline> m_Pipeline;
		Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer> m_IndexBuffer;
		Ref<Texture2D> m_FontTexture;

		std::unordered_map<ImTextureID, Ref<Texture2D>> m_TextureMap;
	};

}