#include "FluxPCH.h"
#include "ImGuiRenderer.h"

#include "Flux/Core/Engine.h"
#include "Renderer.h"

#include <imgui.h>




// Temp
#include <glad/glad.h>

namespace Flux {

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

		// Backend
		{
			io.BackendPlatformUserData = this;
			io.BackendPlatformName = "WindowsImGuiImplementation";
			io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
			io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
			io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;
			io.BackendFlags |= ImGuiBackendFlags_HasMouseHoveredViewport;

			auto& window = Engine::Get().GetWindow();
			WindowHandle windowHandle = window->GetNativeHandle();

			ImGuiViewport* mainViewport = ImGui::GetMainViewport();
			mainViewport->PlatformHandle = windowHandle;
			mainViewport->PlatformHandleRaw = windowHandle;

			ImGuiPlatformIO& platformIO = ImGui::GetPlatformIO();
			// platformIO.Platform_CreateWindow = ImGui_CreateWindow;
		}

		// TODO: HACK
		// Submit immediately to render thread
		// and wait
		auto& renderThread = Engine::Get().GetRenderThread();
		renderThread->Submit([this]()
		{
			ImGuiIO& io = ImGui::GetIO();

			uint8* pixels;
			int32 width, height;
			io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

			glCreateTextures(GL_TEXTURE_2D, 1, &m_FontTextureID);
			glTextureParameteri(m_FontTextureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(m_FontTextureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
			glTextureStorage2D(m_FontTextureID, 1, GL_RGBA8, width, height);
			glTextureSubImage2D(m_FontTextureID, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

			io.Fonts->SetTexID((ImTextureID)(uintptr)m_FontTextureID);
		});
		renderThread->Wait();

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

	void ImGuiRenderer::BeginFrame()
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		auto& window = Engine::Get().GetWindow();

		ImGuiIO& io = ImGui::GetIO();

		io.DisplaySize = { (float)window->GetWidth(), (float)window->GetHeight() };
		io.DeltaTime = Engine::Get().GetFrameTime();

		// Win32 NewFrame
		// Graphics API NewFrame

		ImGui::NewFrame();
	}

	void ImGuiRenderer::EndFrame()
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

			for (int32 commandListIndex = 0; commandListIndex < drawData->CmdListsCount; commandListIndex++)
			{
				const ImDrawList* commandList = drawData->CmdLists[commandListIndex];

				const uint32 vertexBufferSize = commandList->VtxBuffer.Size * sizeof(ImDrawVert);
				if (!m_VertexBuffer)
					m_VertexBuffer = VertexBuffer::Create(vertexBufferSize, VertexBufferUsage::Stream);
				if (vertexBufferSize> m_VertexBuffer->GetSize())
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

						FLUX_SUBMIT_RENDER_COMMAND([textureID = command->GetTexID()]()
						{
							glBindTexture(GL_TEXTURE_2D, (uint32)(intptr)textureID);
						});

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