#pragma once

#include "Flux/Runtime/Core/Engine.h"

#include "Project/Project.h"

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
		void CreateWindowMenus();

		void OnWindowMenuEvent(WindowMenuEvent& event);
		void OnWindowCloseEvent(WindowCloseEvent& event);

		void OpenProject();
		void OpenProject(const std::filesystem::path& path);
		void NewProject();
		void SaveProject();
		void CloseProject();
	private:
		Ref<Project> m_Project;

		Ref<Texture> m_ViewportPlaceholderTexture;
		uint8* m_ViewportPlaceholderTextureData = nullptr;
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