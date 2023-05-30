#include "FluxPCH.h"
#include "EditorEngine.h"

namespace Flux {

	void EditorEngine::OnInit()
	{
		SubmitToEventThread([this]()
		{
			WindowMenu fileMenu = m_Window->CreateMenu();
			m_Window->AddMenu(fileMenu, Menu_File_NewProject, "New Project...", true);
			m_Window->AddMenu(fileMenu, Menu_File_OpenProject, "Open Project...");
			m_Window->AddMenu(fileMenu, Menu_File_SaveProject, "Save Project", true);
			m_Window->AddMenuSeparator(fileMenu);
			m_Window->AddMenu(fileMenu, Menu_File_Restart, "Restart");
			m_Window->AddMenu(fileMenu, Menu_File_Exit, "Exit\tAlt+F4");

			WindowMenu editMenu = m_Window->CreateMenu();
			m_Window->AddMenu(editMenu, Menu_Edit_Preferences, "Preferences");

			WindowMenu aboutMenu = m_Window->CreateMenu();
			m_Window->AddMenu(aboutMenu, Menu_About_AboutFluxEngine, "About Flux Engine");

			WindowMenu menu = m_Window->CreateMenu();
			m_Window->AddPopupMenu(menu, fileMenu, "File");
			m_Window->AddPopupMenu(menu, editMenu, "Edit");
			m_Window->AddPopupMenu(menu, aboutMenu, "About");

			m_Window->SetMenu(menu);
			m_Window->AddMenuCallback(FLUX_BIND_CALLBACK(OnMenuCallback, this));

			WindowCreateInfo windowCreateInfo;
			windowCreateInfo.ParentWindow = m_Window.get();

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
		});

		m_RenderPipeline = Ref<ForwardRenderPipeline>::Create();
	}

	void EditorEngine::OnExit()
	{
		FLUX_VERIFY(m_RenderPipeline->GetReferenceCount() == 1);
		m_RenderPipeline = nullptr;

		SubmitToEventThread([this]()
		{
			m_Windows.clear();
		});
	}

	void EditorEngine::OnUpdate()
	{
		float width = Engine::Get().GetSwapchain()->GetWidth();
		float height = Engine::Get().GetSwapchain()->GetHeight();

		m_RenderPipeline->BeginRendering2D();

		float scale = 20.0f;
		float x = (width / 2.0f) - (scale / 2.0f);
		float y = (height / 2.0f) - (scale / 2.0f);

		int32 skip = 0;

		for (float xa = -width * 0.5f; xa < width + width * 1.5f; xa += width / 20.0f)
		{
			skip = 0;

			for (float ya = -height * 0.5f; ya < height + height * 1.5f; ya += height / 20.0f)
			{
				float xo = xa + scale * 0.5f;
				float yo = ya + scale * 0.5f;

				xo += glm::sin(Platform::GetTime() * 2.0f) * ya * 0.1f;
				yo += glm::cos(Platform::GetTime() * 2.0f) * xa * 0.1f;

				skip++;
				if (skip < 5)
					m_RenderPipeline->DrawQuad({ xo, yo, 0.0f }, { scale, scale }, { xa / x, ya / y, (xa + ya) / (x + y), 1.0f });
				if (skip > 10)
					skip = 0;
			}
		}

		m_RenderPipeline->EndRendering2D();
	}

	void EditorEngine::OnMenuCallback(WindowMenu menu, uint32 itemID)
	{
		switch (itemID)
		{
		case Menu_File_OpenProject:
		{
			std::string outPath;
			if (Platform::OpenFolderDialog(m_Window.get(), &outPath, "Load Project") == DialogResult::Ok)
				FLUX_ERROR("Path: {0}", outPath);
			break;
		}
		case Menu_File_Restart:
		{
			SubmitToMainThread([this]() { Close(true); });
			break;
		}
		case Menu_File_Exit:
		{
			SubmitToMainThread([this]() { Close(); });
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