#include "FluxPCH.h"
#include "EditorEngine.h"

#include "Flux/Runtime/Renderer/Renderer.h"

#include <glad/glad.h>

namespace Flux {

	EditorEngine::EditorEngine(const EngineCreateInfo& createInfo)
		: Engine(createInfo)
	{
		CreateWindowMenus();
	}

	EditorEngine::~EditorEngine()
	{
		FLUX_CHECK_IS_IN_EVENT_THREAD();
	}

	void EditorEngine::OnInit()
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		uint32 width = m_MainWindow->GetWidth();
		uint32 height = m_MainWindow->GetHeight();
		m_EditorCamera.SetViewportSize(width, height);

		m_Shader = Shader::Create("Resources/Shaders/Shader.glsl");

		GraphicsPipelineCreateInfo pipelineCreateInfo;
		pipelineCreateInfo.VertexDeclaration = {
			{ "a_Position", VertexElementFormat::Float3 },
			{ "a_Normal", VertexElementFormat::Float3 },
			{ "a_Tangent", VertexElementFormat::Float3 },
			{ "a_Binormal", VertexElementFormat::Float3 },
			{ "a_TexCoord", VertexElementFormat::Float2 }
		};
		pipelineCreateInfo.DepthTest = true;
		pipelineCreateInfo.DepthWrite = true;
		pipelineCreateInfo.BackfaceCulling = true;
		m_Pipeline = GraphicsPipeline::Create(pipelineCreateInfo);

		FramebufferCreateInfo framebufferCreateInfo;
		framebufferCreateInfo.Attachments = { TextureFormat::RGBA32, TextureFormat::Depth24Stencil8 };
		m_Framebuffer = Framebuffer::Create(framebufferCreateInfo);

		m_SphereMesh = Mesh::LoadFromFile("Resources/Meshes/Sphere.glb");

		// m_CubemapTexture = TextureLoader::LoadTextureFromFile("Resources/Textures/newport_loft.hdr");

		{
			uint32 whiteTextureData = 0xFFFFFFFF;

			TextureProperties properties;
			properties.Width = 1;
			properties.Height = 1;
			properties.Format = TextureFormat::RGBA32;
			m_WhiteTexture = Texture::Create(properties, &whiteTextureData);
		}

		{
			TextureProperties properties;
			properties.Width = 16;
			properties.Height = 16;
			properties.Format = TextureFormat::RGBA32;
			m_CheckerboardTexture = Texture::Create(properties);

			for (uint32 y = 0; y < m_CheckerboardTexture->GetProperties().Height; y++)
			{
				for (uint32 x = 0; x < m_CheckerboardTexture->GetProperties().Width; x++)
				{
					uint32 color = (x + y % 2 == 0) ? 0xFFFFFFFF : 0xFF808080;
					m_CheckerboardTexture->SetPixel(x, y, color);
				}
			}
			m_CheckerboardTexture->Apply();
		}

		// Default material
		{
#if 0
			m_Material.AlbedoMap = TextureLoader::LoadTextureFromFile("Resources/Textures/rustediron2/rustediron2_basecolor.png");
			m_Material.NormalMap = TextureLoader::LoadTextureFromFile("Resources/Textures/rustediron2/rustediron2_normal.png");
			m_Material.RoughnessMap = TextureLoader::LoadTextureFromFile("Resources/Textures/rustediron2/rustediron2_roughness.png");
			m_Material.MetalnessMap = TextureLoader::LoadTextureFromFile("Resources/Textures/rustediron2/rustediron2_metallic.png");
#else
			m_Material.AlbedoMap = m_WhiteTexture;
			m_Material.NormalMap = m_WhiteTexture;
			m_Material.RoughnessMap = m_WhiteTexture;
			m_Material.MetalnessMap = m_WhiteTexture;
#endif

			m_Material.AlbedoColor = Vector4(1.0f);
			m_Material.Metalness = 0.0f;
			m_Material.Roughness = 0.5f;
			m_Material.Emission = 0.0f;
			m_Material.Name = "Default Material";
		}

		OpenProject();
	}

	void EditorEngine::OnShutdown()
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		CloseProject();
	}

	void EditorEngine::OnUpdate()
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

#if 0
		// Clear color (TODO: remove)
		FLUX_SUBMIT_RENDER_COMMAND([windowWidth = m_MainWindow->GetWidth(), windowHeight = m_MainWindow->GetHeight()]() mutable
		{
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

			glDisable(GL_SCISSOR_TEST);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_SCISSOR_TEST);

			glViewport(0, 0, windowWidth, windowHeight);
		});
#endif

		// TODO
		float deltaTime = m_DeltaTime;
		m_EditorCamera.OnUpdate(deltaTime);

		if (m_ViewportWidth > 0 && m_ViewportHeight > 0)
		{
			m_EditorCamera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
			m_Framebuffer->Resize(m_ViewportWidth, m_ViewportHeight);

			m_Framebuffer->Bind();
			{
				Quaternion lightRotation = Quaternion(m_LightRotation * Math::DegToRad);
				Vector3 lightDirection = lightRotation * Vector3(0.0f, 0.0f, 1.0f);

				m_Shader->Bind();
				m_Shader->SetUniform("u_LightColor", m_LightColor);
				m_Shader->SetUniform("u_AmbientMultiplier", m_AmbientMultiplier);
				m_Shader->SetUniform("u_ViewMatrix", m_EditorCamera.GetViewMatrix());
				m_Shader->SetUniform("u_ViewProjectionMatrix", m_EditorCamera.GetProjectionMatrix() * m_EditorCamera.GetViewMatrix());
				m_Shader->SetUniform("u_CameraPosition", m_EditorCamera.GetPosition());
				m_Shader->SetUniform("u_LightDirection", lightDirection);

				uint32 numSpheresX = 7;
				uint32 numSpheresY = 7;
				float spacing = 2.5f;

				for (uint32 x = 0; x < numSpheresX; x++)
				{
					MaterialDescriptor material = m_Material;
					material.Roughness = Math::Clamp((float)x / (float)numSpheresX, 0.05f, 1.0f);

					for (uint32 y = 0; y < numSpheresY; y++)
					{
						material.Metalness = (float)y / (float)numSpheresY;

						Matrix4x4 transform = Math::BuildTransformationMatrix({
							(float)(x - ((float)numSpheresX * 0.5f)) * spacing,
							(float)(y - ((float)numSpheresY * 0.5f)) * spacing, 0.0f
							}, Vector3(0.0f));
						RenderMeshWithMaterial(m_SphereMesh, material, transform);
					}
				}
			}
			m_Framebuffer->Unbind();
		}
	}

	static Entity s_SelectedEntity;

	void EditorEngine::DrawEntityHierarchy(Entity entity)
	{
		ImGuiTreeNodeFlags flags = 0;
		flags |= ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		if (s_SelectedEntity == entity)
			flags |= ImGuiTreeNodeFlags_Selected;
		if (!entity.HasChildren())
			flags |= ImGuiTreeNodeFlags_Leaf;

		bool open = ImGui::TreeNodeEx(entity.GetName().c_str(), flags);

		if (ImGui::BeginDragDropSource())
		{
			EntityID entityID = entity;
			ImGui::SetDragDropPayload("Hierarchy_Entity", &entityID, sizeof(EntityID));
			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Hierarchy_Entity"))
			{
				EntityID entityID = *(EntityID*)payload->Data;

				Entity childEntity = { entityID, &m_Scene };
				childEntity.SetParent(entity);
			}
			ImGui::EndDragDropTarget();
		}

		if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
			s_SelectedEntity = entity;

		if (open)
		{
			for (Entity childEntity : entity.GetChildren())
				DrawEntityHierarchy(childEntity);

			ImGui::TreePop();
		}
	}

	void EditorEngine::RenderMesh(Ref<Mesh> mesh, const Matrix4x4& transform)
	{
		mesh->GetVertexBuffer()->Bind();
		m_Pipeline->Bind();
		m_Pipeline->Scissor(0, 0, m_ViewportWidth, m_ViewportHeight);
		mesh->GetIndexBuffer()->Bind();

		m_Shader->Bind();

		auto& properties = m_SphereMesh->GetProperties();
		for (size_t i = 0; i < properties.Submeshes.size(); i++)
		{
			auto& submesh = properties.Submeshes[i];
			m_Shader->SetUniform("u_Transform", transform * submesh.WorldTransform);

			auto& material = properties.Materials[submesh.MaterialIndex];
			m_Shader->SetUniform("u_AlbedoColor", material.AlbedoColor);
			m_Shader->SetUniform("u_Roughness", material.Roughness);
			m_Shader->SetUniform("u_Metalness", material.Metalness);
			m_Shader->SetUniform("u_Emission", material.Emission);

			bool hasNormalMap = !material.NormalMap.Equals(m_WhiteTexture);
			m_Shader->SetUniform("u_HasNormalMap", uint32(hasNormalMap ? 1 : 0));

			if (material.AlbedoMap)
			{
				material.AlbedoMap->Bind(0);
				m_Shader->SetUniform("u_AlbedoMap", 0);
			}

			if (material.NormalMap)
			{
				material.NormalMap->Bind(1);
				m_Shader->SetUniform("u_NormalMap", 1);
			}

			if (material.RoughnessMap)
			{
				material.RoughnessMap->Bind(2);
				m_Shader->SetUniform("u_RoughnessMap", 2);
			}

			if (material.MetalnessMap)
			{
				material.MetalnessMap->Bind(3);
				m_Shader->SetUniform("u_MetalnessMap", 3);
			}

			m_Pipeline->DrawIndexed(
				submesh.IndexFormat,
				submesh.IndexCount,
				submesh.StartIndexLocation,
				submesh.BaseVertexLocation
			);

			if (material.AlbedoMap)
				material.AlbedoMap->Unbind(0);
			if (material.NormalMap)
				material.NormalMap->Unbind(1);
			if (material.RoughnessMap)
				material.RoughnessMap->Unbind(2);
			if (material.MetalnessMap)
				material.MetalnessMap->Unbind(3);
		}
	}

	void EditorEngine::RenderMeshWithMaterial(Ref<Mesh> mesh, const MaterialDescriptor& material, const Matrix4x4& transform)
	{
		mesh->GetVertexBuffer()->Bind();
		m_Pipeline->Bind();
		m_Pipeline->Scissor(0, 0, m_ViewportWidth, m_ViewportHeight);
		mesh->GetIndexBuffer()->Bind();

		m_Shader->Bind();

		auto& properties = m_SphereMesh->GetProperties();
		for (size_t i = 0; i < properties.Submeshes.size(); i++)
		{
			auto& submesh = properties.Submeshes[i];
			m_Shader->SetUniform("u_Transform", transform * submesh.WorldTransform);

			m_Shader->SetUniform("u_AlbedoColor", material.AlbedoColor);
			m_Shader->SetUniform("u_Roughness", material.Roughness);
			m_Shader->SetUniform("u_Metalness", material.Metalness);
			m_Shader->SetUniform("u_Emission", material.Emission);

			bool hasNormalMap = !material.NormalMap.Equals(m_WhiteTexture);
			m_Shader->SetUniform("u_HasNormalMap", uint32(hasNormalMap ? 1 : 0));

			if (material.AlbedoMap)
			{
				material.AlbedoMap->Bind(0);
				m_Shader->SetUniform("u_AlbedoMap", 0);
			}

			if (material.NormalMap)
			{
				material.NormalMap->Bind(1);
				m_Shader->SetUniform("u_NormalMap", 1);
			}

			if (material.RoughnessMap)
			{
				material.RoughnessMap->Bind(2);
				m_Shader->SetUniform("u_RoughnessMap", 2);
			}

			if (material.MetalnessMap)
			{
				material.MetalnessMap->Bind(3);
				m_Shader->SetUniform("u_MetalnessMap", 3);
			}

			m_Pipeline->DrawIndexed(
				submesh.IndexFormat,
				submesh.IndexCount,
				submesh.StartIndexLocation,
				submesh.BaseVertexLocation
			);

			if (material.AlbedoMap)
				material.AlbedoMap->Unbind(0);
			if (material.NormalMap)
				material.NormalMap->Unbind(1);
			if (material.RoughnessMap)
				material.RoughnessMap->Unbind(2);
			if (material.MetalnessMap)
				material.MetalnessMap->Unbind(3);
		}
	}

	void EditorEngine::OnImGuiRender()
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		ImGuiWindowFlags dockspaceFlags = 0;
		dockspaceFlags |= ImGuiWindowFlags_NoDocking;
		dockspaceFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;
		dockspaceFlags |= ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		dockspaceFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		static bool open = true;
		ImGui::Begin("MainDockspace", &open, dockspaceFlags);
		ImGui::PopStyleVar(3);

		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspaceID = ImGui::GetID("MainDockspace");
			ImGui::DockSpace(dockspaceID);
		}

		ImGui::End();

		ImGui::Begin("Properties");

		ImGui::ColorEdit4("Albedo Color", m_Material.AlbedoColor.GetPointer());

		ImGui::DragFloat("Ambient Multiplier", &m_AmbientMultiplier, 0.001f, 0.0f, 1.0f);

		ImGui::DragFloat3("Light Rotation", m_LightRotation.GetPointer());
		Quaternion lightRotation = Quaternion(m_LightRotation * Math::DegToRad);
		Vector3 lightDirection = lightRotation * Vector3(0.0f, 0.0f, 1.0f);
		ImGui::Text("Light Direction: [%.2f, %.2f, %.2f]", lightDirection.X, lightDirection.Y, lightDirection.Z);

		ImGui::ColorEdit3("Light Color", m_LightColor.GetPointer());

		ImGui::Text("Camera Position: [%.2f, %.2f, %.2f]", m_EditorCamera.GetPosition().X, m_EditorCamera.GetPosition().Y, m_EditorCamera.GetPosition().Z);
		ImGui::Text("Camera Rotation: [%.2f, %.2f, %.2f]", m_EditorCamera.GetRotation().X, m_EditorCamera.GetRotation().Y, m_EditorCamera.GetRotation().Z);

		ImGui::End();

		ImGui::Begin("Hierarchy");

		ImVec2 minRegion = ImGui::GetWindowContentRegionMin();
		ImVec2 maxRegion = ImGui::GetWindowContentRegionMax();
		ImVec2 windowPos = ImGui::GetWindowPos();

		ImVec2 bounds[2];
		bounds[0] = { minRegion.x + windowPos.x, minRegion.y + windowPos.y };
		bounds[1] = { maxRegion.x + windowPos.x, maxRegion.y + windowPos.y };

		if (ImGui::Button("+"))
		{
			static uint32 entityIndex = 0;
			entityIndex++;
			m_Scene.CreateEntity(fmt::format("Entity {0}", entityIndex));
		}

		for (Entity entity : m_Scene.GetRootEntities())
			DrawEntityHierarchy(entity);

		if (ImGui::BeginDragDropTargetCustom({ bounds[0].x, bounds[0].y, bounds[1].x, bounds[1].y }, ImGui::GetCurrentWindow()->ID))
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Hierarchy_Entity"))
			{
				EntityID entityID = *(EntityID*)payload->Data;
				Entity entity = { entityID, &m_Scene };
				entity.Unparent();
			}
			
			ImGui::EndDragDropTarget();
		}

		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
		ImGui::Begin("Viewport", nullptr, ImGuiWindowFlags_NoScrollbar);

		int32 viewportWidth = (int32)ImGui::GetContentRegionAvail().x;
		int32 viewportHeight = (int32)ImGui::GetContentRegionAvail().y;

		if (viewportWidth < 0)
			viewportWidth = 0;
		if (viewportHeight < 0)
			viewportHeight = 0;

		if (m_ViewportWidth != viewportWidth || m_ViewportHeight != viewportHeight)
		{
			m_ViewportWidth = viewportWidth;
			m_ViewportHeight = viewportHeight;

			if (m_ViewportWidth > 0 && m_ViewportHeight > 0)
			{
				// on resize
			}
		}

		m_ImGuiRenderer->Image(m_Framebuffer->GetColorAttachment(), { (float)m_ViewportWidth, (float)m_ViewportHeight }, { 0.0f, 1.0f }, { 1.0f, 0.0f });

		ImGui::End();
		ImGui::PopStyleVar();

		ImGui::Begin("Flux Engine");

		BuildConfiguration buildConfig = Engine::GetBuildConfiguration();
		const char* buildConfigString = Utils::BuildConfigurationToString(buildConfig);
		ImGui::Text("%s build", buildConfigString);
		ImGui::Separator();

		ImGui::Text("Ticks per second: %d", m_TicksPerSecond);
		ImGui::Text("Events per second: %d", m_EventsPerSecond);
		ImGui::Separator();

		ImGui::Checkbox("V-Sync", &m_VSync);
		ImGui::Text("%d fps", m_FramesPerSecond);
		ImGui::Text("Delta Time: %.2fms", m_DeltaTime * 1000.0f);
		// ImGui::DragFloat("Fixed Delta Time", &m_FixedDeltaTime, 0.001f, 0.001f, 0.1f);
		ImGui::Text("Fixed Delta Time: %.2f", m_FixedDeltaTime);

		ImGui::Separator();
		ImGui::Text("Command queues: %d", Renderer::GetQueueCount());

		ImGui::BeginDisabled();
		bool multithreaded = m_RenderThread != nullptr;
		ImGui::Checkbox("Multithreaded", &multithreaded);
		ImGui::EndDisabled();

		if (m_RenderThread)
		{
			ImGui::Separator();
			ImGui::Text("Render Thread wait: %.2fms", m_RenderThreadWaitTime);
		}

		ImGui::End();
	}

	void EditorEngine::CreateWindowMenus()
	{
		FLUX_CHECK_IS_IN_EVENT_THREAD();

		WindowMenu fileMenu = m_MainWindow->CreateMenu();
		m_MainWindow->AddMenu(fileMenu, Menu_File_NewProject, "New Project...");
		m_MainWindow->AddMenu(fileMenu, Menu_File_OpenProject, "Open Project...");
		m_MainWindow->AddMenu(fileMenu, Menu_File_SaveProject, "Save Project");
		m_MainWindow->AddMenuSeparator(fileMenu);
		m_MainWindow->AddMenu(fileMenu, Menu_File_Restart, "Restart");
		m_MainWindow->AddMenu(fileMenu, Menu_File_Exit, "Exit\tAlt+F4");

		WindowMenu editMenu = m_MainWindow->CreateMenu();
		m_MainWindow->AddMenu(editMenu, Menu_Edit_Preferences, "Preferences");

		WindowMenu aboutMenu = m_MainWindow->CreateMenu();
		m_MainWindow->AddMenu(aboutMenu, Menu_About_AboutFluxEngine, "About Flux Engine");

		WindowMenu menu = m_MainWindow->CreateMenu();
		m_MainWindow->AddPopupMenu(menu, fileMenu, "File");
		m_MainWindow->AddPopupMenu(menu, editMenu, "Edit");
		m_MainWindow->AddPopupMenu(menu, aboutMenu, "About");

		WindowCreateInfo windowCreateInfo;
		windowCreateInfo.ParentWindow = m_MainWindow;

		// About
		windowCreateInfo.Title = "About Flux Engine";
		windowCreateInfo.Width = 610;
		windowCreateInfo.Height = 410;
		windowCreateInfo.Resizable = false;
		m_Windows[Menu_About_AboutFluxEngine] = Window::Create(windowCreateInfo);

		// Preferences
		windowCreateInfo.Title = "Preferences";
		windowCreateInfo.Width = 960;
		windowCreateInfo.Height = 540;
		windowCreateInfo.Resizable = true;
		m_Windows[Menu_Edit_Preferences] = Window::Create(windowCreateInfo);

		for (auto& [type, window] : m_Windows)
			window->SetEventQueue(m_EventQueue);

		m_MainWindow->SetMenu(menu);
	}

	void EditorEngine::OnEvent(Event& event)
	{
		EventHandler handler(event);
		handler.Bind<WindowMenuEvent>(FLUX_BIND_CALLBACK(OnWindowMenuEvent, this));
		handler.Bind<WindowCloseEvent>(FLUX_BIND_CALLBACK(OnWindowCloseEvent, this));
	}

	void EditorEngine::OnWindowMenuEvent(WindowMenuEvent& event)
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		MenuItem item = static_cast<MenuItem>(event.GetItemID());

		SubmitToEventThread([this, item]()
		{
			auto it = m_Windows.find(item);
			if (it != m_Windows.end())
				it->second->SetVisible(true);
		});

		switch (item)
		{
		case Menu_File_NewProject:
		{
			NewProject();
			break;
		}
		case Menu_File_OpenProject:
		{
			OpenProject();
			break;
		}
		case Menu_File_SaveProject:
		{
			SaveProject();
			break;
		}
		case Menu_File_Restart:
		{
			Engine::Get().Close(true);
			break;
		}
		case Menu_File_Exit:
		{
			Engine::Get().Close();
			break;
		}
		}
	}

	void EditorEngine::OnWindowCloseEvent(WindowCloseEvent& event)
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		SubmitToEventThread([this, window = event.GetWindow()]()
		{
			for (auto& entry : m_Windows)
			{
				if (entry.second == window)
					window->SetVisible(false);
			}
		});
	}

	void EditorEngine::OpenProject()
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		std::string path;
		// while (path.empty())
		Platform::OpenFolderDialog(nullptr, &path, "Load Project");

		OpenProject(path);
	}

	void EditorEngine::OpenProject(const std::filesystem::path& path)
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		if (!std::filesystem::exists(path))
			return;

		if (m_Project && path == m_Project->GetProjectDirectory())
			return;

		CloseProject();

		FLUX_INFO("Opening project: {0}", path.filename().stem().string());

		m_Project = Project::Load(path);
	}

	void EditorEngine::NewProject()
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		std::string path;
		Platform::OpenFolderDialog(nullptr, &path, "Choose location for new project");

		if (!path.empty())
		{
			CloseProject();

			// m_Project = Ref<Project>::Create(path + '/' + ...)
		}
	}

	void EditorEngine::SaveProject()
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		if (m_Project)
		{
			FLUX_INFO("Saving project: {0}", m_Project->GetName());

			m_Project->SaveSettings();
		}
	}

	void EditorEngine::CloseProject()
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		if (m_Project)
		{
			FLUX_INFO("Saving and closing project: {0}", m_Project->GetName());

			m_Project->SaveSettings();

			FLUX_VERIFY(m_Project->GetReferenceCount() == 1);
			m_Project = nullptr;
		}
	}

}