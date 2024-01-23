#pragma once

#include "Shader.h"
#include "GraphicsPipeline.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Texture.h"

#include "Flux/Runtime/Core/Events/WindowEvent.h"
#include "Flux/Runtime/Core/Events/KeyEvent.h"
#include "Flux/Runtime/Core/Events/MouseEvent.h"

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

		void OnEvent(Event& event) const;
	private:
		void OnWindowResizeEvent(WindowResizeEvent& event) const;
		void OnWindowFocusEvent(WindowFocusEvent& event) const;
		void OnKeyPressedEvent(KeyPressedEvent& event) const;
		void OnKeyReleasedEvent(KeyReleasedEvent& event) const;
		void OnKeyTypedEvent(KeyTypedEvent& event) const;
		void OnMouseMovedEvent(MouseMovedEvent& event) const;
		void OnMouseButtonPressedEvent(MouseButtonPressedEvent& event) const;
		void OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& event) const;
		void OnMouseScrolledEvent(MouseScrolledEvent& event) const;
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