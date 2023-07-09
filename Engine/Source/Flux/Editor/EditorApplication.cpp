#include "FluxPCH.h"
#include "EditorApplication.h"

#include "Flux/Runtime/Core/Engine.h"
#include "Flux/Runtime/Renderer/Renderer.h"

namespace Flux {

	EditorApplication::EditorApplication()
		: Application("Flux Editor")
	{
		auto& window = Engine::Get().GetWindow();

		WindowMenu fileMenu = window->CreateMenu();
		window->AddMenu(fileMenu, Menu_File_NewProject, "New Project...", true);
		window->AddMenu(fileMenu, Menu_File_OpenProject, "Open Project...");
		window->AddMenu(fileMenu, Menu_File_SaveProject, "Save Project", true);
		window->AddMenuSeparator(fileMenu);
		window->AddMenu(fileMenu, Menu_File_Restart, "Restart");
		window->AddMenu(fileMenu, Menu_File_Exit, "Exit\tAlt+F4");

		WindowMenu editMenu = window->CreateMenu();
		window->AddMenu(editMenu, Menu_Edit_Preferences, "Preferences");

		WindowMenu aboutMenu = window->CreateMenu();
		window->AddMenu(aboutMenu, Menu_About_AboutFluxEngine, "About Flux Engine");

		WindowMenu menu = window->CreateMenu();
		window->AddPopupMenu(menu, fileMenu, "File");
		window->AddPopupMenu(menu, editMenu, "Edit");
		window->AddPopupMenu(menu, aboutMenu, "About");

		window->SetMenu(menu);
		window->AddMenuCallback(FLUX_BIND_CALLBACK(OnMenuCallback, this));

		window->AddDropCallback([](auto paths, auto count)
		{
			for (uint32 i = 0; i < count; i++)
			{
				const char* path = paths[i];
				FLUX_TRACE("{0}", path);
			}
		});

		WindowCreateInfo windowCreateInfo;
		windowCreateInfo.ParentWindow = window.get();

		// Preferences
		windowCreateInfo.Width = 960;
		windowCreateInfo.Height = 540;
		windowCreateInfo.Title = "Preferences";
		windowCreateInfo.Resizable = true;
		m_Windows[MenuItem::Menu_Edit_Preferences] = Window::Create(windowCreateInfo);

		// About
		windowCreateInfo.Width = 640;
		windowCreateInfo.Height = 410;
		windowCreateInfo.Title = "About Flux Engine";
		windowCreateInfo.Resizable = false;
		m_Windows[MenuItem::Menu_About_AboutFluxEngine] = Window::Create(windowCreateInfo);

		for (auto& [type, window] : m_Windows)
		{
			if (window)
			{
				window->AddCloseCallback([this, type]()
				{
					auto it = m_Windows.find(type);
					if (it != m_Windows.end())
						it->second->SetVisible(false);
				});
			}
		}

		m_OrthoCamera.SetViewportSize(window->GetWidth(), window->GetHeight());
		m_OrthoCamera.SetZoomLevel(7.0f);
	}

	EditorApplication::~EditorApplication()
	{
	}

	void EditorApplication::OnInit()
	{
		m_RenderPipeline = Ref<ForwardRenderPipeline>::Create();
		
		FramebufferCreateInfo framebufferCreateInfo;
		framebufferCreateInfo.Attachments = {
			PixelFormat::RGBA,
			PixelFormat::Depth24Stencil8
		};
		framebufferCreateInfo.SwapchainTarget = true;
		framebufferCreateInfo.DebugLabel = "Swapchain";

		GraphicsPipelineCreateInfo pipelineCreateInfo;
		pipelineCreateInfo.Framebuffer = Framebuffer::Create(framebufferCreateInfo);
		pipelineCreateInfo.Shader = Renderer::GetShader("Blit");
		pipelineCreateInfo.DepthWrite = false;
		pipelineCreateInfo.DebugLabel = "Swapchain Pass";
		m_SwapchainPipeline = GraphicsPipeline::Create(pipelineCreateInfo);

		m_SwapchainMaterial = RenderMaterial::Create(pipelineCreateInfo.Shader, "Swapchain-Blit");

		CommandBufferCreateInfo commandBufferCreateInfo;
		commandBufferCreateInfo.Transient = true;
		commandBufferCreateInfo.CreateFromSwapchain = true;
		commandBufferCreateInfo.DebugLabel = "Swapchain-CommandBuffer";
		m_SwapchainCommandBuffer = CommandBuffer::Create(commandBufferCreateInfo);

		m_Texture = Ref<Texture2D>::Create("Resources/Textures/Islandox.png");
	}

	void EditorApplication::OnExit()
	{
#if FLUX_SEPARATE_FRAMEBUFFERS
		FLUX_VERIFY(m_SwapchainPipeline->GetReferenceCount() == 1);
		m_SwapchainPipeline = nullptr;
#endif

		FLUX_VERIFY(m_RenderPipeline->GetReferenceCount() == 1);
		m_RenderPipeline = nullptr;

		FLUX_VERIFY(m_Texture->GetReferenceCount() == 1);
		m_Texture = nullptr;
	}

	void EditorApplication::OnUpdate()
	{
		uint32 width = Engine::Get().GetSwapchain()->GetWidth();
		uint32 height = Engine::Get().GetSwapchain()->GetHeight();
		m_RenderPipeline->SetViewportSize(width, height);
		m_OrthoCamera.SetViewportSize(width, height);

		auto& cameraSettings = m_RenderPipeline->GetCameraSettings();
		cameraSettings.ViewMatrix = m_OrthoCamera.ViewMatrix;
		cameraSettings.ProjectionMatrix = m_OrthoCamera.ProjectionMatrix;
		cameraSettings.NearClip = -1.0f;
		cameraSettings.FarClip = 1.0f;

		m_RenderPipeline->BeginRendering2D();
		m_RenderPipeline->DrawQuad({ 0.0f, 0.0f, 0.0f }, { 5.0f, 5.0f });
		m_RenderPipeline->EndRendering2D();

		m_SwapchainCommandBuffer->Begin();

		Renderer::BeginRenderPass(m_SwapchainCommandBuffer, m_SwapchainPipeline);
		auto finalImage = m_RenderPipeline->GetComposedImage();
		if (finalImage)
		{
			m_SwapchainMaterial->Set("u_Texture", finalImage);
			Renderer::RenderFullscreenQuad(m_SwapchainCommandBuffer, m_SwapchainMaterial);
		}
		Renderer::EndRenderPass(m_SwapchainCommandBuffer);

		m_SwapchainCommandBuffer->End();
		m_SwapchainCommandBuffer->Submit();
	}

	void EditorApplication::OnMenuCallback(WindowMenu menu, uint32 itemID)
	{
		auto& window = Engine::Get().GetWindow();

		switch (itemID)
		{
		case Menu_File_OpenProject:
		{
			std::string outPath;
			if (Platform::OpenFolderDialog(window.get(), &outPath, "Load Project") == DialogResult::Ok)
				FLUX_ERROR("Path: {0}", outPath);
			break;
		}
		case Menu_File_Restart:
		{
			Engine::Get().SubmitToMainThread([this]() { Engine::Get().Close(true); });
			break;
		}
		case Menu_File_Exit:
		{
			Engine::Get().SubmitToMainThread([this]() { Engine::Get().Close(); });
			break;
		}
		default:
		{
			auto it = m_Windows.find(static_cast<MenuItem>(itemID));
			if (it != m_Windows.end())
				it->second->SetVisible(true);
			break;
		}
		}
	}

}