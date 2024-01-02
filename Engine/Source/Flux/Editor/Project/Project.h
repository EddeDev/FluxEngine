#pragma once

namespace Flux {

	class Project : public ReferenceCounted
	{
	public:
		Project(const std::filesystem::path& path);
		~Project();

		void SaveSettings();
		
		const std::filesystem::path& GetProjectDirectory() const { return m_ProjectDirectory; }
		const std::filesystem::path& GetProjectSettingsDirectory() const { return m_ProjectSettingsDirectory; }
		const std::filesystem::path& GetAssetsDirectory() const { return m_AssetsDirectory; }

		const std::string& GetName() const { return m_Name; }

		static Ref<Project> Load(const std::filesystem::path& path);
		static Ref<Project> GetActive() { return s_ActiveProject; }
	private:
		void CreateDirectory(const std::filesystem::path& path);
	private:
		std::string m_Name;

		std::filesystem::path m_ProjectDirectory;
		std::filesystem::path m_ProjectSettingsDirectory;
		std::filesystem::path m_AssetsDirectory;

		inline static Project* s_ActiveProject = nullptr;
	};

}