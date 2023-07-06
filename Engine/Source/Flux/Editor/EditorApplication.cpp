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

		struct Quad
		{
			glm::vec2 Position = { 0.0f, 0.0f };
			glm::vec2 Scale = { 1.0f, 1.0f };

			glm::vec4 Color = { 0.8f, 0.4f, 0.2f, 1.0f };

			bool Dragging = false;

			bool ResizingLeft = false;
			bool ResizingRight = false;
			bool ResizingBottom = false;
			bool ResizingTop = false;

			static bool HoveringRect(const glm::vec2& position, const glm::vec2& scale, const glm::vec2& mouseOrthoPos)
			{
				float left = position.x - scale.x * 0.5f;
				float right = position.x + scale.x * 0.5f;
				float bottom = position.y - scale.y * 0.5f;
				float top = position.y + scale.y * 0.5f;

				return mouseOrthoPos.x >= left &&
					mouseOrthoPos.x <= right &&
					mouseOrthoPos.y >= bottom &&
					mouseOrthoPos.y <= top;
			}

			bool Hovering(const glm::vec2& mouseOrthoPos) const
			{
				return HoveringRect(Position, Scale, mouseOrthoPos);
			}

			void Update(Ref<RenderPipeline> pipeline, const glm::vec2& mouseOrthoPos)
			{
				float left = Position.x - Scale.x * 0.5f;
				float right = Position.x + Scale.x * 0.5f;
				float bottom = Position.y - Scale.y * 0.5f;
				float top = Position.y + Scale.y * 0.5f;

				if (!Dragging)
				{
					glm::vec4 resizeGripColor = Color * glm::vec4(0.5f);

					// Left side
					{
						glm::vec2 position = { left - 0.025f, Position.y };
						glm::vec2 scale = { 0.1f, top - bottom };

						bool hovering = HoveringRect(position, scale, mouseOrthoPos);
						if (hovering)
						{
							if (Input::GetMouseButtonDown(MouseButtonCode::ButtonLeft))
								ResizingLeft = true;
						}

						if (hovering || ResizingLeft)
						{
							pipeline->DrawQuad({ position, 0.1f }, scale, resizeGripColor);
						}

						if (ResizingLeft)
						{
							float delta = Input::GetMousePositionDelta().x * 0.04f;
							Scale.x -= delta;

							if (Input::GetMouseButtonUp(MouseButtonCode::ButtonLeft))
								ResizingLeft = false;
						}
					}

					// Right side
					{
						glm::vec2 position = { right + 0.025f, Position.y };
						glm::vec2 scale = { 0.1f, top - bottom };

						bool hovering = HoveringRect(position, scale, mouseOrthoPos);
						if (hovering)
						{
							if (Input::GetMouseButtonDown(MouseButtonCode::ButtonLeft))
								ResizingRight = true;
						}

						if (hovering || ResizingRight)
						{
							pipeline->DrawQuad({ position, 0.1f }, scale, resizeGripColor);
						}

						if (ResizingRight)
						{
							float delta = Input::GetMousePositionDelta().x * 0.04f;
							Scale.x += delta;

							if (Input::GetMouseButtonUp(MouseButtonCode::ButtonLeft))
								ResizingRight = false;
						}
					}

					// Bottom side
					{
						glm::vec2 position = { Position.x, bottom - 0.025f };
						glm::vec2 scale = { right - left, 0.1f };

						bool hovering = HoveringRect(position, scale, mouseOrthoPos);
						if (hovering)
						{
							if (Input::GetMouseButtonDown(MouseButtonCode::ButtonLeft))
								ResizingBottom = true;
						}

						if (hovering || ResizingBottom)
						{
							pipeline->DrawQuad({ position, 0.1f }, scale, resizeGripColor);
						}

						if (ResizingBottom)
						{
							float delta = Input::GetMousePositionDelta().y * 0.04f;
							Scale.y += delta;

							if (Input::GetMouseButtonUp(MouseButtonCode::ButtonLeft))
								ResizingBottom = false;
						}
					}

					// Top side
					{
						glm::vec2 position = { Position.x, top + 0.025f };
						glm::vec2 scale = { right - left, 0.1f };

						bool hovering = HoveringRect(position, scale, mouseOrthoPos);
						if (hovering)
						{
							if (Input::GetMouseButtonDown(MouseButtonCode::ButtonLeft))
								ResizingTop = true;
						}

						if (hovering || ResizingTop)
						{
							pipeline->DrawQuad({ position, 0.1f }, scale, resizeGripColor);
						}

						if (ResizingTop)
						{
							float delta = Input::GetMousePositionDelta().y * 0.04f;
							Scale.y -= delta;

							if (Input::GetMouseButtonUp(MouseButtonCode::ButtonLeft))
								ResizingTop = false;
						}
					}
				}

				pipeline->DrawQuad(glm::vec3(Position, 0.0f), Scale, Color);

				if (!Dragging)
				{
					if (Hovering(mouseOrthoPos))
					{
						if (Input::GetMouseButtonDown(MouseButtonCode::ButtonLeft))
							Dragging = true;
					}
				}

				if (ResizingLeft || ResizingRight || ResizingBottom || ResizingTop)
					Dragging = false;
				
				if (Dragging)
				{
					Position = glm::lerp(Position, mouseOrthoPos, 40.0f * Engine::Get().GetFrameTime());
				
					if (Input::GetMouseButtonUp(MouseButtonCode::ButtonLeft))
						Dragging = false;
				}
			}
		};

		glm::vec2 mouseOrthoPos = Input::GetMouseOrthoPosition(m_OrthoCamera.ViewMatrix, m_OrthoCamera.ProjectionMatrix);

		static std::vector<Quad> quads;

		static bool firstFrame = true;
		if (firstFrame)
		{
			quads.emplace_back();
			firstFrame = false;
		}

		if (Input::GetKeyDown(KeyCode::Q))
		{
			auto& quad = quads.emplace_back();

			float r1 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
			float r2 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
			float r3 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
			quad.Color = { r1, r2, r3, 1.0f };

			float r4 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
			float r5 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
			quad.Position = { r4 * 5.0f - 2.5f, r5 * 2.0f - 1.0f };
		}

		for (auto& quad : quads)
			quad.Update(m_RenderPipeline, mouseOrthoPos);

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