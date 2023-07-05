#include "FluxPCH.h"
#include "EditorApplication.h"

#include "Flux/Runtime/Core/Engine.h"

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
		m_Texture = Ref<Texture2D>::Create("Resources/Textures/Islandox.png");
	}

	void EditorApplication::OnExit()
	{
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

		auto& cameraSettings = m_RenderPipeline->GetCameraSettings();
		cameraSettings.ViewMatrix = m_OrthoCamera.ViewMatrix;
		cameraSettings.ProjectionMatrix = m_OrthoCamera.ProjectionMatrix;
		cameraSettings.NearClip = -1.0f;
		cameraSettings.FarClip = 1.0f;

		m_RenderPipeline->BeginRendering2D();

		static glm::vec2 quadPosition = { 0.0f, 0.0f };
		static glm::vec2 quadScale = { 5.0f, 1.0f };
		static glm::vec4 initialQuadColor = { 0.8f, 0.4f, 0.2f, 1.0f };
		static glm::vec4 quadColor = initialQuadColor;

#if 0
		static float moveSpeed = 4.5f;

		if (Input::GetKey(KeyCode::W))
			quadPosition.y += moveSpeed * Engine::Get().GetFrameTime();
		if (Input::GetKey(KeyCode::S))
			quadPosition.y -= moveSpeed * Engine::Get().GetFrameTime();

		if (Input::GetKey(KeyCode::A))
			quadPosition.x -= moveSpeed * Engine::Get().GetFrameTime();
		if (Input::GetKey(KeyCode::D))
			quadPosition.x += moveSpeed * Engine::Get().GetFrameTime();
#endif

		glm::vec2 mouseOrthoPos = Input::GetMouseOrthoPosition(m_OrthoCamera.ViewMatrix, m_OrthoCamera.ProjectionMatrix);

		float left = quadPosition.x - quadScale.x * 0.5f;
		float right = quadPosition.x + quadScale.x * 0.5f;
		float bottom = quadPosition.y - quadScale.y * 0.5f;
		float top = quadPosition.y + quadScale.y * 0.5f;

		static bool draggingQuad = false;

		float hoveringQuad = mouseOrthoPos.x >= left && mouseOrthoPos.x <= right && mouseOrthoPos.y >= bottom && mouseOrthoPos.y <= top;
		if (hoveringQuad)
		{
			quadColor = glm::vec4(initialQuadColor.r * 0.8f, initialQuadColor.g * 0.8f, initialQuadColor.b * 0.8f, 1.0f);
			
			if (Input::GetMouseButtonDown(MouseButtonCode::ButtonLeft))
				draggingQuad = true;
		}
		else
		{
			quadColor = initialQuadColor;
		}

		if (draggingQuad)
		{
			quadPosition = glm::lerp(quadPosition, mouseOrthoPos, 40.0f * Engine::Get().GetFrameTime());
		
			if (Input::GetMouseButtonUp(MouseButtonCode::ButtonLeft))
				draggingQuad = false;
		}

		if (Input::GetKeyUp(KeyCode::Space))
		{
			FLUX_INFO("SPACE!");
		}

		m_RenderPipeline->DrawQuad(glm::vec3(quadPosition, 0.0f), quadScale, quadColor);

		m_RenderPipeline->EndRendering2D();
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