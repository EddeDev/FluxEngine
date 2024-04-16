#pragma once

#include "EditorWindow.h"

#include "Flux/Runtime/Asset/AssetMetadata.h"

namespace Flux {

	class ProjectBrowserWindow : public EditorWindow
	{
	public:
		ProjectBrowserWindow();
		virtual ~ProjectBrowserWindow();

		virtual void OnImGuiRender() override;
	private:
		std::filesystem::path m_CurrentDirectory;
		std::filesystem::path m_AssetDirectory;
	};

}