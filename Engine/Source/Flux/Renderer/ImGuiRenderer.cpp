#include "FluxPCH.h"
#include "ImGuiRenderer.h"

#include "Flux/Core/Engine.h"
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

	ImGuiRenderer::ImGuiRenderer()
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		IMGUI_CHECKVERSION();

		m_Context = ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();
		io.IniFilename = "Config/ImGui.ini";
		io.LogFilename = "Logs/ImGui.log";
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		
		auto& window = Engine::Get().GetWindow();
		io.DisplaySize = ImVec2(window->GetWidth(), window->GetHeight());

		io.BackendPlatformUserData = this;
		io.BackendPlatformName = "FluxEngine";
		io.BackendRendererName = "FluxEngine";
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
		io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;
		io.BackendFlags |= ImGuiBackendFlags_HasMouseHoveredViewport;

		Engine::Get().SubmitToEventThread([]()
		{
			auto& window = Engine::Get().GetWindow();
			window->AddSizeCallback([](auto width, auto height)
			{
				Engine::Get().SubmitToMainThread([width, height]()
				{
					ImGuiIO& io = ImGui::GetIO();
					io.DisplaySize = ImVec2(width, height);
					io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
				});
			});
			window->AddFocusCallback([](auto focused)
			{
				Engine::Get().SubmitToMainThread([focused]()
				{
					ImGuiIO& io = ImGui::GetIO();
					io.AddFocusEvent(focused);
				});
			});
			window->AddKeyCallback([](auto key, auto scancode, auto action, auto mods)
			{
				Engine::Get().SubmitToMainThread([key, scancode, action, mods]()
				{
					if (action != FLUX_ACTION_PRESS && action != FLUX_ACTION_RELEASE)
						return;

					ImGuiIO& io = ImGui::GetIO();
					io.AddKeyEvent(Utils::KeyCodeToImGuiKey(key), action == FLUX_ACTION_PRESS);
				});
			});
			window->AddCharCallback([](auto codepoint)
			{
				Engine::Get().SubmitToMainThread([codepoint]()
				{
					ImGuiIO& io = ImGui::GetIO();
					io.AddInputCharacter(codepoint);
				});
			});
			window->AddMouseMoveCallback([](auto x, auto y)
			{
				Engine::Get().SubmitToMainThread([x, y]()
				{
					ImGuiIO& io = ImGui::GetIO();
					io.AddMousePosEvent(x, y);
				});
			});
			window->AddMouseButtonCallback([](auto button, auto action, auto mods)
			{
				Engine::Get().SubmitToMainThread([button, action, mods]()
				{
					ImGuiIO& io = ImGui::GetIO();
					io.AddMouseButtonEvent(button, action == FLUX_ACTION_PRESS);
				});
			});
			window->AddMouseWheelCallback([](auto x, auto y)
			{
				Engine::Get().SubmitToMainThread([x, y]()
				{
					ImGuiIO& io = ImGui::GetIO();
					io.AddMouseWheelEvent(x, y);
				});
			});
		});

		WindowHandle windowHandle = window->GetNativeHandle();

		ImGuiViewport* mainViewport = ImGui::GetMainViewport();
		mainViewport->PlatformHandle = window.get();
		mainViewport->PlatformHandleRaw = windowHandle;

		ImGuiPlatformIO& platformIO = ImGui::GetPlatformIO();
		// platformIO.Platform_CreateWindow = ImGui_CreateWindow;

		// Font texture
		{
			uint8* pixels;
			int32 width, height, bytesPerPixel;
			io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, &bytesPerPixel);

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
		FLUX_CHECK_IS_MAIN_THREAD();

		ImGui::DestroyContext(m_Context);
	}

	void ImGuiRenderer::NewFrame()
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		auto& window = Engine::Get().GetWindow();

		ImGuiIO& io = ImGui::GetIO();
		io.DeltaTime = Engine::Get().GetFrameTime();

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
		FLUX_CHECK_IS_MAIN_THREAD();

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

				const float projectionMatrix[4][4] =
				{
					{ 2.0f / (right - left),           0.0f,                            0.0f, 0.0f },
					{ 0.0f,                            2.0f / (top - bottom),           0.0f, 0.0f },
					{ 0.0f,                            0.0f,                           -1.0f, 0.0f },
					{ (right + left) / (left - right), (top + bottom) / (bottom - top), 0.0f, 1.0f },
				};

				m_Shader->SetUniformMatrix4x4("u_ProjectionMatrix", &projectionMatrix[0][0]);
			}

			for (int32 commandListIndex = 0; commandListIndex < drawData->CmdListsCount; commandListIndex++)
			{
				const ImDrawList* commandList = drawData->CmdLists[commandListIndex];

				const uint32 vertexBufferSize = commandList->VtxBuffer.Size * sizeof(ImDrawVert);
				if (!m_VertexBuffer)
					m_VertexBuffer = VertexBuffer::Create(vertexBufferSize, VertexBufferUsage::Stream);
				if (vertexBufferSize > m_VertexBuffer->GetSize())
					m_VertexBuffer->Resize(vertexBufferSize);
				m_VertexBuffer->SetData(commandList->VtxBuffer.Data, vertexBufferSize);

				const uint32 indexBufferSize = commandList->IdxBuffer.Size * sizeof(ImDrawIdx);
				if (!m_IndexBuffer)
					m_IndexBuffer = IndexBuffer::Create(indexBufferSize, sizeof(ImDrawIdx) == 2 ? IndexBufferDataType::UInt16 : IndexBufferDataType::UInt32, IndexBufferUsage::Stream);
				if (indexBufferSize > m_IndexBuffer->GetSize())
					m_IndexBuffer->Resize(indexBufferSize);
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

						m_FontTexture->Bind();

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

		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
	}

}