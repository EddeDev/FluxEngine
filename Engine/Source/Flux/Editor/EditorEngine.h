#pragma once

#include "Flux/Runtime/Engine/Engine.h"
#include "Flux/Runtime/Renderer/RenderPipeline.h"

namespace Flux {

	class EditorEngine : public Engine
	{
	public:
		EditorEngine() {}

		virtual void OnInit() override;
		virtual void OnExit() override;
		virtual void OnUpdate() override;
	private:
		void OnMenuCallback(WindowMenu menu, uint32 menuID);
	private:
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

		std::unordered_map<MenuItem, Unique<Window>> m_Windows;

		Ref<RenderPipeline> m_RenderPipeline;
	};

}