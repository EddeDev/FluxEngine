#pragma once

#include "Flux/Runtime/Core/Engine.h"
#include "Flux/Runtime/Renderer/Mesh.h"

#include "Project/Project.h"
#include "EditorCamera.h"

#include "Flux/Runtime/Scene/Scene.h"
#include "Flux/Runtime/Scene/Entity.h"

namespace Flux {

	class EditorEngine : public Engine
	{
	public:
		EditorEngine(const EngineCreateInfo& createInfo);
		virtual ~EditorEngine();
	protected:
		virtual void OnInit() override;
		virtual void OnShutdown() override;
		virtual void OnUpdate() override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event& event) override;
	private:
		void DrawEntityHierarchy(Entity entity);

		void RenderMesh(Ref<Mesh> mesh, const Matrix4x4& transform);
		void RenderMeshWithMaterial(Ref<Mesh> mesh, const MaterialDescriptor& material, const Matrix4x4& transform);

		void CreateWindowMenus();

		void OnWindowMenuEvent(WindowMenuEvent& event);
		void OnWindowCloseEvent(WindowCloseEvent& event);

		void OpenProject();
		void OpenProject(const std::filesystem::path& path);
		void NewProject();
		void SaveProject();
		void CloseProject();
	private:
		Ref<Shader> m_Shader;
		Ref<GraphicsPipeline> m_Pipeline;
		Ref<Framebuffer> m_Framebuffer;
		Ref<Mesh> m_SphereMesh;
		Ref<Texture> m_CubemapTexture;
		Ref<Texture> m_WhiteTexture;
		Ref<Texture> m_CheckerboardTexture;
		MaterialDescriptor m_Material;
		EditorCamera m_EditorCamera;
		Vector3 m_LightColor = Vector3(0.9f, 0.87f, 0.96f);
		Vector3 m_LightRotation = Vector3(50.0f, -30.0f, 0.0f);
		float m_AmbientMultiplier = 0.0f;

		Ref<Project> m_Project;

		Scene m_Scene;

		uint32 m_ViewportWidth = 0;
		uint32 m_ViewportHeight = 0;

		enum MenuItem : uint32
		{
			// File
			Menu_File_NewProject,
			Menu_File_OpenProject,
			Menu_File_SaveProject,
			Menu_File_Restart,
			Menu_File_Exit,

			// Edit
			Menu_Edit_Preferences,

			// About
			Menu_About_AboutFluxEngine
		};

		std::unordered_map<MenuItem, Ref<Window>> m_Windows;
	};

}