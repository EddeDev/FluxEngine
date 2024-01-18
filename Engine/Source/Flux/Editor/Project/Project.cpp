#include "FluxPCH.h"
#include "Project.h"

namespace Flux {

	Project::Project(const std::filesystem::path& path)
	{
		FLUX_VERIFY(!s_ActiveProject);
		FLUX_VERIFY(std::filesystem::is_directory(path));

		s_ActiveProject = this;

		m_Name = path.filename().stem().string();

		m_ProjectDirectory = path;
		m_ProjectSettingsDirectory = path / "ProjectSettings";
		m_AssetsDirectory = path / "Assets";

		CreateDirectory(m_ProjectDirectory);
		CreateDirectory(m_ProjectSettingsDirectory);
		CreateDirectory(m_AssetsDirectory);
	}

	Project::~Project()
	{
		s_ActiveProject = nullptr;
	}

	void Project::SaveSettings()
	{

	}

	Ref<Project> Project::Load(const std::filesystem::path& path)
	{
		return Ref<Project>::Create(path);
	}

	void Project::CreateDirectory(const std::filesystem::path& path)
	{
		if (!std::filesystem::exists(path))
		{
			FLUX_TRACE("Creating directory: {0}", path.string());
			std::filesystem::create_directories(path);
		}
	}
}