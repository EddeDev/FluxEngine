#include "FluxPCH.h"
#include "ProjectBrowserWindow.h"

#include "Project/Project.h"
#include "EditorAssetDatabase.h"

#include "Flux/Runtime/Core/Engine.h"
#include "Flux/Runtime/Utils/StringUtils.h"

#include "Flux/Runtime/ImGui/ImGuiUtils.h"
#include "Flux/Runtime/ImGui/FontAwesome.h"

#include <imgui.h>
#include <imgui_internal.h>

namespace Flux {

	ProjectBrowserWindow::ProjectBrowserWindow()
	{
	}

	ProjectBrowserWindow::~ProjectBrowserWindow()
	{
	}

	void ProjectBrowserWindow::OnImGuiRender()
	{
		Ref<Project> project = Project::GetActive();
		if (!project)
			return;

		auto& assetDirectory = project->GetAssetsDirectory();
		if (m_AssetDirectory != assetDirectory)
		{
			m_CurrentDirectory = assetDirectory;
			m_AssetDirectory = assetDirectory;
		}

		while (!std::filesystem::exists(m_CurrentDirectory))
			m_CurrentDirectory = m_CurrentDirectory.parent_path();

		Ref<EditorAssetDatabase> assetDatabase = Project::GetActive()->GetAssetDatabase<EditorAssetDatabase>();

		auto& metadataMap = assetDatabase->GetMetadataMap();
		for (auto& [assetID, metadata] : metadataMap)
		{
			std::string s = metadata.RelativeAssetPath.string();
			ImGui::Text(s.c_str());
		}
	}

}