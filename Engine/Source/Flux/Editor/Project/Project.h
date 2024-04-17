#pragma once

#include "Flux/Runtime/Asset/AssetDatabaseInterface.h"

namespace Flux {

	struct ProjectSettings
	{
		Vector3 EditorCameraPosition = Vector3(0.0f);
		Vector3 EditorCameraRotation = Vector3(0.0f);
	};

	class Project : public ReferenceCounted
	{
	public:
		Project(const std::filesystem::path& path);
		~Project();

		void LoadFromFile();
		void SaveToFile();

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
		const std::filesystem::path& GetAssetsDirectory() const { return m_AssetsDirectory; }

		const std::string& GetName() const { return m_Name; }

		ProjectSettings& GetSettings() { return m_Settings; }

		static Ref<Project> LoadFromFile(const std::filesystem::path& path);
		static Ref<Project> GetActive() { return s_ActiveProject; }
	private:
		void CreateDirectory(const std::filesystem::path& path);
	private:
		std::string m_Name;
		ProjectSettings m_Settings;

		std::filesystem::path m_ProjectDirectory;
		std::filesystem::path m_AssetsDirectory;

		Ref<AssetDatabaseInterface> m_AssetDatabase;

		inline static Project* s_ActiveProject = nullptr;
	};

}