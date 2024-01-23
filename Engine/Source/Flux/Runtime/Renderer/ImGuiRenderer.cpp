#include "FluxPCH.h"
#include "ImGuiRenderer.h"

#include "Flux/Runtime/Core/Engine.h"
#include "Renderer.h"

#include <imgui.h>

namespace Flux {

	namespace Utils {

		static ImGuiKey KeyCodeToImGuiKey(int32 key)
		{
			switch (key)
			{
			case FLUX_KEY_SPACE: return ImGuiKey_Space;
			case FLUX_KEY_APOSTROPHE: return ImGuiKey_Apostrophe;
			case FLUX_KEY_COMMA: return ImGuiKey_Comma;
			case FLUX_KEY_MINUS: return ImGuiKey_Minus;
			case FLUX_KEY_PERIOD: return ImGuiKey_Period;
			case FLUX_KEY_SLASH: return ImGuiKey_Slash;
			case FLUX_KEY_0: return ImGuiKey_0;
			case FLUX_KEY_1: return ImGuiKey_1;
			case FLUX_KEY_2: return ImGuiKey_2;
			case FLUX_KEY_3: return ImGuiKey_3;
			case FLUX_KEY_4: return ImGuiKey_4;
			case FLUX_KEY_5: return ImGuiKey_5;
			case FLUX_KEY_6: return ImGuiKey_6;
			case FLUX_KEY_7: return ImGuiKey_7;
			case FLUX_KEY_8: return ImGuiKey_8;
			case FLUX_KEY_9: return ImGuiKey_9;
			case FLUX_KEY_SEMICOLON: return ImGuiKey_Semicolon;
			case FLUX_KEY_EQUAL: return ImGuiKey_Equal;
			case FLUX_KEY_A: return ImGuiKey_A;
			case FLUX_KEY_B: return ImGuiKey_B;
			case FLUX_KEY_C: return ImGuiKey_C;
			case FLUX_KEY_D: return ImGuiKey_D;
			case FLUX_KEY_E: return ImGuiKey_E;
			case FLUX_KEY_F: return ImGuiKey_F;
			case FLUX_KEY_G: return ImGuiKey_G;
			case FLUX_KEY_H: return ImGuiKey_H;
			case FLUX_KEY_I: return ImGuiKey_I;
			case FLUX_KEY_J: return ImGuiKey_J;
			case FLUX_KEY_K: return ImGuiKey_K;
			case FLUX_KEY_L: return ImGuiKey_L;
			case FLUX_KEY_M: return ImGuiKey_M;
			case FLUX_KEY_N: return ImGuiKey_N;
			case FLUX_KEY_O: return ImGuiKey_O;
			case FLUX_KEY_P: return ImGuiKey_P;
			case FLUX_KEY_Q: return ImGuiKey_Q;
			case FLUX_KEY_R: return ImGuiKey_R;
			case FLUX_KEY_S: return ImGuiKey_S;
			case FLUX_KEY_T: return ImGuiKey_T;
			case FLUX_KEY_U: return ImGuiKey_U;
			case FLUX_KEY_V: return ImGuiKey_V;
			case FLUX_KEY_W: return ImGuiKey_W;
			case FLUX_KEY_X: return ImGuiKey_X;
			case FLUX_KEY_Y: return ImGuiKey_Y;
			case FLUX_KEY_Z: return ImGuiKey_Z;
			case FLUX_KEY_LEFT_BRACKET: return ImGuiKey_LeftBracket;
			case FLUX_KEY_BACKSLASH: return ImGuiKey_Backslash;
			case FLUX_KEY_RIGHT_BRACKET: return ImGuiKey_RightBracket;
			case FLUX_KEY_GRAVE_ACCENT: return ImGuiKey_GraveAccent;
			case FLUX_KEY_ESCAPE: return ImGuiKey_Escape;
			case FLUX_KEY_ENTER: return ImGuiKey_Enter;
			case FLUX_KEY_TAB: return ImGuiKey_Tab;
			case FLUX_KEY_BACKSPACE: return ImGuiKey_Backspace;
			case FLUX_KEY_INSERT: return ImGuiKey_Insert;
			case FLUX_KEY_DELETE: return ImGuiKey_Delete;
			case FLUX_KEY_RIGHT: return ImGuiKey_RightArrow;
			case FLUX_KEY_LEFT: return ImGuiKey_LeftArrow;
			case FLUX_KEY_DOWN: return ImGuiKey_DownArrow;
			case FLUX_KEY_UP: return ImGuiKey_UpArrow;
			case FLUX_KEY_PAGE_UP: return ImGuiKey_PageUp;
			case FLUX_KEY_PAGE_DOWN: return ImGuiKey_PageDown;
			case FLUX_KEY_HOME: return ImGuiKey_Home;
			case FLUX_KEY_END: return ImGuiKey_End;
			case FLUX_KEY_CAPS_LOCK: return ImGuiKey_CapsLock;
			case FLUX_KEY_SCROLL_LOCK: return ImGuiKey_ScrollLock;
			case FLUX_KEY_NUM_LOCK: return ImGuiKey_NumLock;
			case FLUX_KEY_PRINT_SCREEN: return ImGuiKey_PrintScreen;
			case FLUX_KEY_PAUSE: return ImGuiKey_Pause;
			case FLUX_KEY_F1: return ImGuiKey_F1;
			case FLUX_KEY_F2: return ImGuiKey_F2;
			case FLUX_KEY_F3: return ImGuiKey_F3;
			case FLUX_KEY_F4: return ImGuiKey_F4;
			case FLUX_KEY_F5: return ImGuiKey_F5;
			case FLUX_KEY_F6: return ImGuiKey_F6;
			case FLUX_KEY_F7: return ImGuiKey_F7;
			case FLUX_KEY_F8: return ImGuiKey_F8;
			case FLUX_KEY_F9: return ImGuiKey_F9;
			case FLUX_KEY_F10: return ImGuiKey_F10;
			case FLUX_KEY_F11: return ImGuiKey_F11;
			case FLUX_KEY_F12: return ImGuiKey_F12;
			case FLUX_KEY_F13: return ImGuiKey_F13;
			case FLUX_KEY_F14: return ImGuiKey_F14;
			case FLUX_KEY_F15: return ImGuiKey_F15;
			case FLUX_KEY_F16: return ImGuiKey_F16;
			case FLUX_KEY_F17: return ImGuiKey_F17;
			case FLUX_KEY_F18: return ImGuiKey_F18;
			case FLUX_KEY_F19: return ImGuiKey_F19;
			case FLUX_KEY_F20: return ImGuiKey_F20;
			case FLUX_KEY_F21: return ImGuiKey_F21;
			case FLUX_KEY_F22: return ImGuiKey_F22;
			case FLUX_KEY_F23: return ImGuiKey_F23;
			case FLUX_KEY_F24: return ImGuiKey_F24;
			case FLUX_KEY_KP_0: return ImGuiKey_Keypad0;
			case FLUX_KEY_KP_1: return ImGuiKey_Keypad1;
			case FLUX_KEY_KP_2: return ImGuiKey_Keypad2;
			case FLUX_KEY_KP_3: return ImGuiKey_Keypad3;
			case FLUX_KEY_KP_4: return ImGuiKey_Keypad4;
			case FLUX_KEY_KP_5: return ImGuiKey_Keypad5;
			case FLUX_KEY_KP_6: return ImGuiKey_Keypad6;
			case FLUX_KEY_KP_7: return ImGuiKey_Keypad7;
			case FLUX_KEY_KP_8: return ImGuiKey_Keypad8;
			case FLUX_KEY_KP_9: return ImGuiKey_Keypad9;
			case FLUX_KEY_KP_DECIMAL: return ImGuiKey_KeypadDecimal;
			case FLUX_KEY_KP_DIVIDE: return ImGuiKey_KeypadDivide;
			case FLUX_KEY_KP_MULTIPLY: return ImGuiKey_KeypadMultiply;
			case FLUX_KEY_KP_SUBTRACT: return ImGuiKey_KeypadSubtract;
			case FLUX_KEY_KP_ADD: return ImGuiKey_KeypadAdd;
			case FLUX_KEY_KP_ENTER: return ImGuiKey_KeypadEnter;
			case FLUX_KEY_KP_EQUAL: return ImGuiKey_KeypadEqual;
			case FLUX_KEY_LEFT_SHIFT: return ImGuiKey_LeftShift;
			case FLUX_KEY_LEFT_CONTROL: return ImGuiKey_LeftCtrl;
			case FLUX_KEY_LEFT_ALT: return ImGuiKey_LeftAlt;
			case FLUX_KEY_LEFT_SUPER: return ImGuiKey_LeftSuper;
			case FLUX_KEY_RIGHT_SHIFT: return ImGuiKey_RightSuper;
			case FLUX_KEY_RIGHT_CONTROL: return ImGuiKey_RightCtrl;
			case FLUX_KEY_RIGHT_ALT: return ImGuiKey_RightAlt;
			case FLUX_KEY_RIGHT_SUPER: return ImGuiKey_RightSuper;
			case FLUX_KEY_MENU: return ImGuiKey_Menu;
			}
			return ImGuiKey_None;
		}

		static CursorShape CursorShapeFromImGuiMouseCursor(ImGuiMouseCursor cursor)
		{
			switch (cursor)
			{
			case ImGuiMouseCursor_Arrow:      return CursorShape::Arrow;
			case ImGuiMouseCursor_TextInput:  return CursorShape::IBeam;
			case ImGuiMouseCursor_ResizeAll:  return CursorShape::ResizeAll;
			case ImGuiMouseCursor_ResizeNS:   return CursorShape::ResizeNS;
			case ImGuiMouseCursor_ResizeEW:   return CursorShape::ResizeEW;
			case ImGuiMouseCursor_ResizeNESW: return CursorShape::ResizeNESW;
			case ImGuiMouseCursor_ResizeNWSE: return CursorShape::ResizeNWSE;
			case ImGuiMouseCursor_Hand:       return CursorShape::Hand;
			case ImGuiMouseCursor_NotAllowed: return CursorShape::NotAllowed;
			}
			return CursorShape::None;
		}

	}

	static void ImGui_Platform_CreateWindow(ImGuiViewport* viewport)
	{
		FLUX_INFO_CATEGORY("ImGui", "Creating window...");
	}

	static void ImGui_Platform_DestroyWindow(ImGuiViewport* viewport)
	{
		FLUX_INFO_CATEGORY("ImGui", "Detroying window...");
	}

	static void ImGui_Platform_ShowWindow(ImGuiViewport* viewport)
	{
		FLUX_INFO_CATEGORY("ImGui", "Showing window...");
	}

	static ImVec2 ImGui_Platform_GetWindowPos(ImGuiViewport* viewport)
	{
		return {};
	}

	static void ImGui_Platform_SetWindowPos(ImGuiViewport* viewport, ImVec2 pos)
	{
		FLUX_INFO_CATEGORY("ImGui", "Setting window position...");
	}

	static ImVec2 ImGui_Platform_GetWindowSize(ImGuiViewport* viewport)
	{
		return {};
	}

	static void ImGui_Platform_SetWindowSize(ImGuiViewport* viewport, ImVec2 size)
	{
		FLUX_INFO_CATEGORY("ImGui", "Setting window size...");
	}

	static void ImGui_Platform_SetWindowFocus(ImGuiViewport* viewport)
	{
		FLUX_INFO_CATEGORY("ImGui", "Setting window focus...");
	}

	static bool ImGui_Platform_GetWindowFocus(ImGuiViewport* viewport)
	{
		return true;
	}

	static bool ImGui_Platform_GetWindowMinimized(ImGuiViewport* viewport)
	{
		return false;
	}

	static void ImGui_Platform_SetWindowTitle(ImGuiViewport* viewport, const char* title)
	{
		FLUX_INFO_CATEGORY("ImGui", "Setting window title...");
	}

	static void ImGui_Renderer_CreateWindow(ImGuiViewport* viewport)
	{
	}

	static void ImGui_Renderer_DestroyWindow(ImGuiViewport* viewport)
	{
	}

	static void ImGui_Renderer_SetWindowSize(ImGuiViewport* viewport, ImVec2 size)
	{
	}

	ImGuiRenderer::ImGuiRenderer()
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		IMGUI_CHECKVERSION();

		m_Context = ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
		io.IniFilename = "Config/ImGui.ini";
		io.LogFilename = "Logs/ImGui.log";
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		// io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		Ref<Window> window = Engine::Get().GetMainWindow();
		io.DisplaySize = ImVec2((float)window->GetWidth(), (float)window->GetHeight());

		io.BackendPlatformUserData = this;
		io.BackendPlatformName = "FluxEngine";
		io.BackendRendererName = "FluxEngine";
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
		io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;
		io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;
		io.BackendFlags |= ImGuiBackendFlags_HasMouseHoveredViewport;
		
		io.ConfigWindowsMoveFromTitleBarOnly = true;

#if 0
		ImFontConfig fontConfig;
		fontConfig.PixelSnapH = true;
		io.Fonts->AddFontFromFileTTF("Resources/Fonts/Segoe UI/segoeui.ttf", 16.0f, &fontConfig, io.Fonts->GetGlyphRangesCyrillic());

		auto& style = ImGui::GetStyle();
		style.Alpha = 1.0f;
		style.DisabledAlpha = 0.6f;
		style.WindowPadding = { 8.0f, 8.0f };
		style.WindowRounding = 1.0f;
		style.WindowBorderSize = 1.0f;
		style.WindowMinSize = { 32.0f, 32.0f };
		style.WindowTitleAlign = { 0.02f, 0.5f };
		style.ChildRounding = 1.0f;
		style.ChildBorderSize = 1.0f;
		style.PopupRounding = 0.0f;
		style.PopupBorderSize = 1.0f;
		style.FramePadding = { 4.0f, 3.0f };
		style.FrameRounding = 4.0f;
		style.FrameBorderSize = 1.0f;
		style.ItemSpacing = { 8.0f, 4.0f };
		style.ItemInnerSpacing = { 4.0f, 4.0f };
		style.IndentSpacing = 18.0f;
		style.CellPadding = { 4.0f, 2.0f };
		style.ScrollbarSize = 16.0f;
		style.ScrollbarRounding = 9.0f;
		style.GrabMinSize = 12.0f;
		style.GrabRounding = 2.0f;
		style.TabRounding = 5.0f;
		style.ButtonTextAlign = { 0.5f, 0.5f };
		style.SelectableTextAlign = { 0.0f, 0.0f };

		auto& colors = style.Colors;
		colors[ImGuiCol_Text] = ImVec4(0.779221f, 0.779221f, 0.779221f, 1.0f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.513726f, 0.509804f, 0.513726f, 1.0f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.0823529f, 0.0823529f, 0.0823529f, 1.0f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.141176f, 0.141176f, 0.141176f, 1.0f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.101961f, 0.0980392f, 0.105882f, 1.0f);
		colors[ImGuiCol_Border] = ImVec4(0.203922f, 0.2f, 0.203922f, 1.0f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.06f, 0.06f, 0.06f, 1.0f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.1589f, 0.1608f, 0.1589f, 1.0f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.2392f, 0.2373f, 0.2392f, 1.0f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.0883f, 0.0902f, 0.0883f, 1.0f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.1608f, 0.1588f, 0.1608f, 1.0f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0f, 0.0f, 0.0f, 0.51f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.0f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.341176f, 0.341176f, 0.341176f, 1.0f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.392157f, 0.392157f, 0.392157f, 1.0f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.0f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.25f, 0.58f, 0.7f, 1.0f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.22f, 0.22f, 0.22f, 1.0f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.2902f, 0.2882f, 0.2902f, 1.0f);
		colors[ImGuiCol_Button] = ImVec4(0.22f, 0.22f, 0.22f, 1.0f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.3098f, 0.3078f, 0.3098f, 1.0f);
		colors[ImGuiCol_Header] = ImVec4(0.18f, 0.18f, 0.19f, 1.0f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.23f, 0.23f, 0.24f, 1.0f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.31f, 0.3f, 0.31f, 1.0f);
		colors[ImGuiCol_Separator] = ImVec4(0.196078f, 0.182238f, 0.195916f, 1.0f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.1f, 0.4f, 0.75f, 0.78f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.1f, 0.4f, 0.75f, 1.0f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.2f, 0.760784f, 0.964706f, 0.784314f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
		colors[ImGuiCol_Tab] = ImVec4(0.141176f, 0.141176f, 0.141176f, 1.0f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.141176f, 0.141176f, 0.141176f, 1.0f);
		colors[ImGuiCol_TabActive] = ImVec4(0.141176f, 0.141176f, 0.141176f, 1.0f);
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.141176f, 0.141176f, 0.141176f, 1.0f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.141176f, 0.141176f, 0.141176f, 1.0f);
		colors[ImGuiCol_DockingPreview] = ImVec4(0.71f, 0.66f, 0.14f, 0.7f);
		colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.0f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.0f, 0.43f, 0.35f, 1.0f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.9f, 0.7f, 0.0f, 1.0f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.0f, 0.6f, 0.0f, 1.0f);
		colors[ImGuiCol_TableHeaderBg] = ImVec4(0.1863f, 0.1863f, 0.1882f, 1.0f);
		colors[ImGuiCol_TableBorderStrong] = ImVec4(0.3098f, 0.3098f, 0.3078f, 1.0f);
		colors[ImGuiCol_TableBorderLight] = ImVec4(0.2314f, 0.2314f, 0.2294f, 1.0f);
		colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
		colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.06f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(1.0f, 1.0f, 0.0f, 0.9f);
		colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.0f);
		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.7f);
		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.8f, 0.8f, 0.8f, 0.2f);
		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.8f, 0.8f, 0.8f, 0.35f);
#endif

		WindowHandle windowHandle = window->GetNativeHandle();

		ImGuiViewport* mainViewport = ImGui::GetMainViewport();
		mainViewport->PlatformHandle = window.Get();
		mainViewport->PlatformHandleRaw = windowHandle;

		// mainViewport->PlatformUserData = this;

		ImGuiPlatformIO& platformIO = ImGui::GetPlatformIO();

		// Platform
		platformIO.Platform_CreateWindow = ImGui_Platform_CreateWindow;
		platformIO.Platform_DestroyWindow = ImGui_Platform_DestroyWindow;
		platformIO.Platform_ShowWindow = ImGui_Platform_ShowWindow;
		platformIO.Platform_SetWindowPos = ImGui_Platform_SetWindowPos;
		platformIO.Platform_GetWindowPos = ImGui_Platform_GetWindowPos;
		platformIO.Platform_SetWindowSize = ImGui_Platform_SetWindowSize;
		platformIO.Platform_GetWindowSize = ImGui_Platform_GetWindowSize;
		platformIO.Platform_SetWindowFocus = ImGui_Platform_SetWindowFocus;
		platformIO.Platform_GetWindowFocus = ImGui_Platform_GetWindowFocus;
		platformIO.Platform_GetWindowMinimized = ImGui_Platform_GetWindowMinimized;
		platformIO.Platform_SetWindowTitle = ImGui_Platform_SetWindowTitle;

		ImGuiPlatformMonitor monitor;
		// TODO: TEMP
		monitor.MainSize.x = 1920.0f;
		monitor.MainSize.y = 1080.0f;
		platformIO.Monitors.push_back(monitor);

		// Renderer
		platformIO.Renderer_CreateWindow = ImGui_Renderer_CreateWindow;
		platformIO.Renderer_DestroyWindow = ImGui_Renderer_DestroyWindow;
		platformIO.Renderer_SetWindowSize = ImGui_Renderer_SetWindowSize;

		// Font texture
		{
			uint8* pixels;
			int32 width, height, bytesPerPixel;
			io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, &bytesPerPixel);
			io.Fonts->SetTexID(nullptr);

			TextureFormat format = TextureFormat::None;
			if (bytesPerPixel == 4)
				format = TextureFormat::RGBA32;
			else
				FLUX_VERIFY(false);

			m_FontTexture = Texture2D::Create(width, height, format);
			m_FontTexture->SetPixelData(pixels, width * height);
		}

		static const char* s_VertexShaderSource =
			"#version 450 core\n"
			"\n"
			"layout(location = 0) in vec2 a_Position;\n"
			"layout(location = 1) in vec2 a_TexCoord;\n"
			"layout(location = 2) in vec4 a_Color;\n"
			"\n"
			"uniform mat4 u_ProjectionMatrix;\n"
			"\n"
			"layout(location = 0) out vec2 v_TexCoord;\n"
			"layout(location = 1) out vec4 v_Color;\n"
			"\n"
			"void main()\n"
			"{\n"
			"    v_TexCoord = a_TexCoord;\n"
			"    v_Color = a_Color;\n"
			"\n"
			"    gl_Position = u_ProjectionMatrix * vec4(a_Position.xy,0,1);\n"
			"}\n";

		static const char* s_FragmentShaderSource =
			"#version 450 core\n"
			"\n"
			"layout (location = 0) out vec4 o_Color;\n"
			"\n"
			"layout(location = 0) in vec2 v_TexCoord;\n"
			"layout(location = 1) in vec4 v_Color;\n"
			"\n"
			"uniform sampler2D u_Texture;\n"
			"\n"
			"void main()\n"
			"{\n"
			"    o_Color = v_Color * texture(u_Texture, v_TexCoord);\n"
			"}\n";

		m_Shader = Shader::Create(s_VertexShaderSource, s_FragmentShaderSource);

		GraphicsPipelineCreateInfo pipelineCreateInfo;
		pipelineCreateInfo.VertexDeclaration = {
			{ "a_Position", VertexElementFormat::Float2 },
			{ "a_TexCoord", VertexElementFormat::Float2 },
			{ "a_Color", VertexElementFormat::UByte4, true }
		};
		m_Pipeline = GraphicsPipeline::Create(pipelineCreateInfo);
	}

	ImGuiRenderer::~ImGuiRenderer()
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		ImGui::DestroyContext(m_Context);
	}

	void ImGuiRenderer::NewFrame()
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		Ref<Window> window = Engine::Get().GetMainWindow();

		ImGuiIO& io = ImGui::GetIO();
		io.DeltaTime = Engine::Get().GetDeltaTime();

		ImGuiMouseCursor cursor = ImGui::GetMouseCursor();

		ImGuiPlatformIO& platformIO = ImGui::GetPlatformIO();
		for (int32 i = 0; i < platformIO.Viewports.Size; i++)
		{
			Window* window = (Window*)platformIO.Viewports[i]->PlatformHandle;
			if (!window)
			{
				FLUX_VERIFY(false);
				continue;
			}

			if (cursor != ImGuiMouseCursor_None)
			{
				Engine::Get().SubmitToEventThread([window, cursor]()
				{
					window->SetCursorShape(Utils::CursorShapeFromImGuiMouseCursor(cursor));
				});
			}
		}

		ImGui::NewFrame();
	}

	void ImGuiRenderer::Render()
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		ImGui::Render();

		ImDrawData* drawData = ImGui::GetDrawData();
		if (drawData)
		{
			int32 viewportWidth = static_cast<uint32>(drawData->DisplaySize.x);
			int32 viewportHeight = static_cast<uint32>(drawData->DisplaySize.y);
			if (viewportWidth <= 0 || viewportHeight <= 0)
				return;

			m_Shader->Bind();

			// Projection
			{
				float left = drawData->DisplayPos.x;
				float right = drawData->DisplayPos.x + drawData->DisplaySize.x;
				float top = drawData->DisplayPos.y;
				float bottom = drawData->DisplayPos.y + drawData->DisplaySize.y;

				Matrix projectionMatrix;
				projectionMatrix.A1 = 2.0f / (right - left);
				projectionMatrix.B1 = 0.0f;
				projectionMatrix.C1 = 0.0f;
				projectionMatrix.D1 = 0.0f;

				projectionMatrix.A2 = 0.0f;
				projectionMatrix.B2 = 2.0f / (top - bottom);
				projectionMatrix.C2 = 0.0f;
				projectionMatrix.D2 = 0.0f;

				projectionMatrix.A3 = 0.0f;
				projectionMatrix.B3 = 0.0f;
				projectionMatrix.C3 = -1.0f;
				projectionMatrix.D3 = 0.0f;

				projectionMatrix.A4 = (right + left) / (left - right);
				projectionMatrix.B4 = (top + bottom) / (bottom - top);
				projectionMatrix.C4 = 0.0f;
				projectionMatrix.D4 = 1.0f;

				m_Shader->SetUniform("u_ProjectionMatrix", projectionMatrix);
			}

			for (int32 commandListIndex = 0; commandListIndex < drawData->CmdListsCount; commandListIndex++)
			{
				const ImDrawList* commandList = drawData->CmdLists[commandListIndex];

				const uint32 vertexBufferSize = commandList->VtxBuffer.Size * sizeof(ImDrawVert);
				if (!m_VertexBuffer || vertexBufferSize > m_VertexBuffer->GetSize())
					m_VertexBuffer = VertexBuffer::Create(vertexBufferSize, VertexBufferUsage::Stream);
				m_VertexBuffer->SetData(commandList->VtxBuffer.Data, vertexBufferSize);

				const uint32 indexBufferSize = commandList->IdxBuffer.Size * sizeof(ImDrawIdx);
				if (!m_IndexBuffer || indexBufferSize > m_IndexBuffer->GetSize())
					m_IndexBuffer = IndexBuffer::Create(indexBufferSize, sizeof(ImDrawIdx) == 2 ? IndexBufferDataType::UInt16 : IndexBufferDataType::UInt32, IndexBufferUsage::Stream);
				m_IndexBuffer->SetData(commandList->IdxBuffer.Data, indexBufferSize);

				m_VertexBuffer->Bind();
				m_Pipeline->Bind();
				m_IndexBuffer->Bind();

				for (int32 commandIndex = 0; commandIndex < commandList->CmdBuffer.Size; commandIndex++)
				{
					const ImDrawCmd* command = &commandList->CmdBuffer[commandIndex];
					if (command->UserCallback)
					{
						if (command->UserCallback == ImDrawCallback_ResetRenderState)
						{
							
						}
						else
						{
							command->UserCallback(commandList, command);
						}
					}
					else
					{
						ImVec2 clipMin(command->ClipRect.x - drawData->DisplayPos.x, command->ClipRect.y - drawData->DisplayPos.y);
						ImVec2 clipMax(command->ClipRect.z - drawData->DisplayPos.x, command->ClipRect.w - drawData->DisplayPos.y);
						
						if (clipMin.x < 0.0f)
							clipMin.x = 0.0f;
						if (clipMin.y < 0.0f)
							clipMin.y = 0.0f;

						if (clipMax.x > (float)viewportWidth)
							clipMax.x = (float)viewportWidth;
						if (clipMax.y > (float)viewportHeight)
							clipMax.y = (float)viewportHeight;

						if (clipMax.x <= clipMin.x || clipMax.y <= clipMin.y)
							continue;

						m_Pipeline->Scissor(
							(int32)clipMin.x,
							(int32)((float)viewportHeight - clipMax.y),
							(int32)(clipMax.x - clipMin.x),
							(int32)(clipMax.y - clipMin.y)
						);

						if (!command->TextureId)
						{
							m_FontTexture->Bind();
						}
						else
						{
							auto it = m_TextureMap.find(command->TextureId);
							if (it != m_TextureMap.end())
								it->second->Bind();
						}

						m_Pipeline->DrawIndexed(
							m_IndexBuffer->GetDataType(),
							command->ElemCount,
							command->IdxOffset,
							command->VtxOffset
						);
					}
				}
			}
		}

		m_TextureMap.clear();

		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
	}

	void ImGuiRenderer::Image(Ref<Texture2D> texture, const ImVec2& size)
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		ImTextureID textureID = (ImTextureID)(uintptr)(m_TextureMap.size() + 1);
		m_TextureMap[textureID] = texture;
		ImGui::Image(textureID, size);
	}

	void ImGuiRenderer::OnEvent(Event& event) const
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		EventHandler handler(event);
		handler.Bind<WindowResizeEvent>(FLUX_BIND_CALLBACK(OnWindowResizeEvent, this));
		handler.Bind<WindowFocusEvent>(FLUX_BIND_CALLBACK(OnWindowFocusEvent, this));
		handler.Bind<KeyPressedEvent>(FLUX_BIND_CALLBACK(OnKeyPressedEvent, this));
		handler.Bind<KeyReleasedEvent>(FLUX_BIND_CALLBACK(OnKeyReleasedEvent, this));
		handler.Bind<KeyTypedEvent>(FLUX_BIND_CALLBACK(OnKeyTypedEvent, this));
		handler.Bind<MouseMovedEvent>(FLUX_BIND_CALLBACK(OnMouseMovedEvent, this));
		handler.Bind<MouseButtonPressedEvent>(FLUX_BIND_CALLBACK(OnMouseButtonPressedEvent, this));
		handler.Bind<MouseButtonReleasedEvent>(FLUX_BIND_CALLBACK(OnMouseButtonReleasedEvent, this));
		handler.Bind<MouseScrolledEvent>(FLUX_BIND_CALLBACK(OnMouseScrolledEvent, this));
	}

	void ImGuiRenderer::OnWindowResizeEvent(WindowResizeEvent& event) const
	{
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2((float)event.GetWidth(), (float)event.GetHeight());
		io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
	}

	void ImGuiRenderer::OnWindowFocusEvent(WindowFocusEvent& event) const
	{
		ImGuiIO& io = ImGui::GetIO();
		io.AddFocusEvent(event.IsFocused());
	}

	void ImGuiRenderer::OnKeyPressedEvent(KeyPressedEvent& event) const
	{
		ImGuiIO& io = ImGui::GetIO();
		io.AddKeyEvent(Utils::KeyCodeToImGuiKey((int32)event.GetKey()), true);
	}

	void ImGuiRenderer::OnKeyReleasedEvent(KeyReleasedEvent& event) const
	{
		ImGuiIO& io = ImGui::GetIO();
		io.AddKeyEvent(Utils::KeyCodeToImGuiKey((int32)event.GetKey()), false);
	}

	void ImGuiRenderer::OnKeyTypedEvent(KeyTypedEvent& event) const
	{
		ImGuiIO& io = ImGui::GetIO();
		io.AddInputCharacter(event.GetCodePoint());
	}

	void ImGuiRenderer::OnMouseMovedEvent(MouseMovedEvent& event) const
	{
		ImGuiIO& io = ImGui::GetIO();
		io.AddMousePosEvent(event.GetX(), event.GetY());
	}

	void ImGuiRenderer::OnMouseButtonPressedEvent(MouseButtonPressedEvent& event) const
	{
		ImGuiIO& io = ImGui::GetIO();
		io.AddMouseButtonEvent((int32)event.GetButton(), true);
	}

	void ImGuiRenderer::OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& event) const
	{
		ImGuiIO& io = ImGui::GetIO();
		io.AddMouseButtonEvent((int32)event.GetButton(), false);
	}

	void ImGuiRenderer::OnMouseScrolledEvent(MouseScrolledEvent& event) const
	{
		ImGuiIO& io = ImGui::GetIO();
		io.AddMouseWheelEvent(event.GetX(), event.GetY());
	}

}