#include "FluxPCH.h"
#include "ImGuiRenderer.h"

#include "Flux/Core/Engine.h"

#include <imgui.h>

namespace Flux {

	ImGuiRenderer::ImGuiRenderer(WindowHandle windowHandle)
	{
		FLUX_CHECK_IS_EVENT_THREAD();

		IMGUI_CHECKVERSION();

		m_Context = ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
		io.IniFilename = "Config/ImGui.ini";
		io.LogFilename = "Logs/ImGui.log";
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		// Backend
		{
			io.BackendPlatformUserData = this;
			io.BackendPlatformName = "WindowsImGuiImplementation";
			io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
			io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
			io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;
			io.BackendFlags |= ImGuiBackendFlags_HasMouseHoveredViewport;

			ImGuiViewport* mainViewport = ImGui::GetMainViewport();
			mainViewport->PlatformHandle = windowHandle;
			mainViewport->PlatformHandleRaw = windowHandle;

			ImGuiPlatformIO& platformIO = ImGui::GetPlatformIO();
			// platformIO.Platform_CreateWindow = ImGui_CreateWindow;
		}
	}

	ImGuiRenderer::~ImGuiRenderer()
	{
		FLUX_CHECK_IS_EVENT_THREAD();

		ImGui::DestroyContext(m_Context);
	}

	void ImGuiRenderer::InitResources()
	{
		FLUX_CHECK_IS_RENDER_THREAD();
	}

	void ImGuiRenderer::BeginFrame()
	{
		FLUX_CHECK_IS_RENDER_THREAD();

		ImGui::NewFrame();
	}

	void ImGuiRenderer::EndFrame()
	{
		FLUX_CHECK_IS_RENDER_THREAD();

		ImGui::Render();

		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
	}

}