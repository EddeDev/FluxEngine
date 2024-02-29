#pragma once

#include "Flux/Runtime/Asset/AssetDatabaseInterface.h"

namespace Flux {

	class Project : public ReferenceCounted
	{
	public:
		Project(const std::filesystem::path& path);
		~Project();

		void SaveSettings();

		template<typename T, typename... TArgs>
		void RegisterAssetDatabase()
		{
			static_assert(std::is_base_of<AssetDatabaseInterface, T>::value);

			m_AssetDatabase = Ref<T>::Create(m_ProjectDirectory, m_AssetsDirectory);
			m_AssetDatabase->ImportAssets();
		}

		void UnregisterAssetDatabase()
		{
			FLUX_VERIFY(m_AssetDatabase->GetReferenceCount() == 1);
			m_AssetDatabase = nullptr;
		}

		template<typename T = AssetDatabaseInterface>
		Ref<T> GetAssetDatabase() const
		{
			static_assert(std::is_base_of<AssetDatabaseInterface, T>::value);
			return m_AssetDatabase.As<T>();
		}
		
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

		Ref<AssetDatabaseInterface> m_AssetDatabase;

		inline static Project* s_ActiveProject = nullptr;
	};

}