#pragma once

#include "Flux/Runtime/Renderer/Shader.h"
#include "Flux/Runtime/Renderer/GraphicsPipeline.h"
#include "Flux/Runtime/Renderer/VertexBuffer.h"
#include "Flux/Runtime/Renderer/IndexBuffer.h"
#include "Flux/Runtime/Renderer/Texture.h"

#include "Flux/Runtime/Core/Events/WindowEvent.h"
#include "Flux/Runtime/Core/Events/KeyEvent.h"
#include "Flux/Runtime/Core/Events/MouseEvent.h"

#include <imgui_internal.h>

namespace Flux {

	enum class FontType : uint8
	{
		None = 0,

		Default,
		FontAwesome
	};

	enum class FontWeight : uint8
	{
		None = 0,

		Regular,
		Italic,
		Bold,
		BoldItalic,
		Light,
		LightItalic,
		Medium,
		MediumItalic,
		Thin,
		ThinItalic,

		// Only for icons
		Solid,
		Brands
	};

	class ImGuiRenderer : public ReferenceCounted
	{
	public:
		ImGuiRenderer();
		virtual ~ImGuiRenderer();

		void NewFrame();
		void Render();

		void Image(Ref<Texture> texture, const ImVec2& size, const ImVec2& uv0 = ImVec2(0.0f, 0.0f), const ImVec2& uv1 = ImVec2(1.0f, 1.0f));

		void OnEvent(Event& event) const;
	
		ImFont* GetFont(FontType type = FontType::Default, FontWeight weight = FontWeight::Regular, uint32 size = 16) const
		{
			auto typeIt = m_Fonts.find(type);
			if (typeIt == m_Fonts.end())
				return nullptr;

			auto weightIt = typeIt->second.find(weight);
			if (weightIt == typeIt->second.end())
				return nullptr;

			auto sizeIt = weightIt->second.find(size);
			if (sizeIt == weightIt->second.end())
				return nullptr;

			if (sizeIt->second)
				return sizeIt->second;

			return ImGui::GetDefaultFont();
		}
	private:
		void AddFont(FontType type, FontWeight weight, std::string_view path);

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
		Ref<Texture> m_FontTexture;

		std::unordered_map<FontType, std::unordered_map<FontWeight, std::unordered_map<uint32, ImFont*>>> m_Fonts;

		std::unordered_map<ImTextureID, Ref<Texture>> m_TextureMap;
	};

}