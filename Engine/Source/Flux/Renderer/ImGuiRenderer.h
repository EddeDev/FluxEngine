#pragma once

#include <imgui_internal.h>

namespace Flux {

	struct ImGuiRendererCreateInfo
	{
		WindowHandle Handle;
	};

	class ImGuiRenderer : public ReferenceCounted
	{
	public:
		ImGuiRenderer(WindowHandle windowHandle);
		virtual ~ImGuiRenderer();

		void InitResources();

		void BeginFrame();
		void EndFrame();
	private:
		ImGuiContext* m_Context = nullptr;
	};

}