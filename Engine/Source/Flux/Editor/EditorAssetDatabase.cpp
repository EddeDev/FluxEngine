#include "FluxPCH.h"
#include "EditorAssetDatabase.h"

#include "Flux/Runtime/Utils/FileHelper.h"

namespace Flux {

	static AssetMetadata s_NullMetadata;
	static Guid s_NullGuid;

	EditorAssetDatabase::EditorAssetDatabase(const std::filesystem::path& projectDirectory, const std::filesystem::path& assetDirectory)
		: m_ProjectDirectory(projectDirectory), m_AssetDirectory(assetDirectory)
	{
		Refresh();
	}

	EditorAssetDatabase::~EditorAssetDatabase()
	{
	}

	Ref<Asset> EditorAssetDatabase::ImportAsset(const Guid& assetID)
	{
		auto memoryAssetIt = m_MemoryAssetMap.find(assetID);
		if (memoryAssetIt != m_MemoryAssetMap.end())
			return memoryAssetIt->second;

		auto metadataIt = m_MetadataMap.find(assetID);
		if (metadataIt == m_MetadataMap.end())
			return nullptr;

		if (!metadataIt->second)
			return nullptr;

		if (!metadataIt->second.IsLoaded)
		{
			Ref<Asset> asset = nullptr;

#if 0
			metadataIt->second.IsLoaded = AssetImporter::Import(metadata, asset);
#endif

			if (!metadataIt->second.IsLoaded)
				return nullptr;

			m_AssetMap[assetID] = asset;
		}

		return m_AssetMap[assetID];
	}

	Ref<Asset> EditorAssetDatabase::GetAssetFromID(const Guid& assetID)
	{
		return ImportAsset(assetID);
	}

	void EditorAssetDatabase::ImportAssets()
	{
		for (auto& [assetID, metadata] : m_MetadataMap)
			ImportAsset(assetID);
	}

	bool EditorAssetDatabase::SaveAsset(const Ref<Asset>& asset)
	{
		if (!asset)
			return false;

		FLUX_VERIFY(!IsMemoryAsset(asset));

		auto& assetID = asset->GetID();

		auto it = m_MetadataMap.find(assetID);
		if (it == m_MetadataMap.end())
			return false;

#if 0
		if (!AssetImporters::Save(it->second, asset))
			return false;
#endif

		if (!SaveMetadata(it->second.RelativeMetaPath))
		{
			FLUX_VERIFY(false);
			return false;
		}

		return true;
	}

	void EditorAssetDatabase::SaveAssets()
	{
		for (auto& [assetID, asset] : m_AssetMap)
			SaveAsset(asset);
	}

	void EditorAssetDatabase::Refresh()
	{
		for (auto& directoryEntry : std::filesystem::recursive_directory_iterator(m_AssetDirectory))
		{
			const auto& path = GetRelativePath(directoryEntry.path());

			bool isMetadata = path.extension().string() == s_AssetMetadataFileExtension;
			if (isMetadata)
			{
				auto assetPath = GetFilesystemPath(GetAssetPath(path));
				if (GetMetadataFromPath(path))
				{
					if (!std::filesystem::exists(assetPath))
					{
						FLUX_INFO_CATEGORY("Asset Database", "Removing metadata {0}", path.string());
						RemoveMetadata(path);
					}
				}
			}
			else
			{
				auto metadataPath = GetRelativePath(GetMetadataPath(path));
				if (!GetMetadataFromPath(metadataPath))
				{
					if (std::filesystem::exists(GetFilesystemPath(metadataPath)))
					{
						FLUX_INFO_CATEGORY("Asset Database", "Importing metadata {0}", metadataPath.string());
						ImportMetadata(metadataPath);
					}
					else
					{
						FLUX_INFO_CATEGORY("Asset Database", "Creating metadata {0}", metadataPath.string());
						CreateMetadata(metadataPath);
					}
				}
			}
		}
	}

	const Guid& EditorAssetDatabase::CreateMetadata(const std::filesystem::path& metadataPath)
	{
		auto assetPath = GetAssetPath(metadataPath);

		Guid assetID = Guid::NewGuid();

		AssetMetadata& metadata = m_MetadataMap[assetID];
		metadata.ID = assetID;
		metadata.Name = assetPath.filename().stem().string();
		metadata.Type = Utils::AssetTypeFromPath(GetFilesystemPath(assetPath));
		metadata.RelativeAssetPath = assetPath;
		metadata.RelativeMetaPath = metadataPath;
		metadata.FilesystemAssetPath = GetFilesystemPath(assetPath);
		metadata.FilesystemMetaPath = GetFilesystemPath(metadataPath);

		SaveMetadata(metadataPath);
		return metadata.ID;
	}

	const Guid& EditorAssetDatabase::ImportMetadata(const std::filesystem::path& metadataPath)
	{
		FLUX_VERIFY(false, "Not implemented!");
		return s_NullMetadata.ID;
	}

	bool EditorAssetDatabase::RemoveMetadata(const std::filesystem::path& metadataPath)
	{
		auto& metadata = GetMetadataFromPath(metadataPath);
		if (!metadata)
			return false;

		auto it = m_MetadataMap.find(metadata.ID);
		if (it == m_MetadataMap.end())
			return false;

		m_MetadataMap.erase(it);

		return std::filesystem::remove(GetFilesystemPath(metadataPath));
	}

	bool EditorAssetDatabase::SaveMetadata(const std::filesystem::path& metadataPath) const
	{
		auto& metadata = GetMetadataFromPath(metadataPath);
		if (!metadata)
		{
			FLUX_VERIFY(false);
			return false;
		}

		return FileHelper::SaveStringToFile("Flux Asset Metadata File", GetFilesystemPath(metadataPath));
	}

	const AssetMetadata& EditorAssetDatabase::GetMetadataFromPath(const std::filesystem::path& metadataPath) const
	{
		auto assetPath = GetAssetPath(metadataPath);
		return GetMetadataFromAssetPath(assetPath);
	}

	const AssetMetadata& EditorAssetDatabase::GetMetadataFromAssetPath(const std::filesystem::path& assetPath) const
	{
		for (auto& [assetID, metadata] : m_MetadataMap)
		{
			if (metadata.RelativeAssetPath == assetPath)
				return metadata;
		}
		return s_NullMetadata;
	}

	const AssetMetadata& EditorAssetDatabase::GetMetadataFromAsset(const Ref<Asset>& asset) const
	{
		auto& assetID = asset->GetID();
		if (m_MetadataMap.find(assetID) != m_MetadataMap.end())
			return m_MetadataMap.at(assetID);
		return s_NullMetadata;
	}

	const AssetMetadata& EditorAssetDatabase::GetMetadataFromAssetID(const Guid& assetID) const
	{
		auto it = m_MetadataMap.find(assetID);
		if (it != m_MetadataMap.end())
			return it->second;
		return s_NullMetadata;
	}

	std::filesystem::path EditorAssetDatabase::GetMetadataPath(const std::filesystem::path& assetPath) const
	{
#ifdef FLUX_ENABLE_ASSERTS
		if (assetPath.has_extension())
		{
			FLUX_ASSERT(assetPath.extension() != s_AssetMetadataFileExtension);
		}
#endif
		return assetPath.string() + s_AssetMetadataFileExtension;
	}

	std::filesystem::path EditorAssetDatabase::GetAssetPath(const std::filesystem::path& metadataPath) const
	{
		std::string pathString = metadataPath.string();
		size_t extensionLength = strlen(s_AssetMetadataFileExtension);
		size_t count = pathString.size() - extensionLength;
		return GetRelativePath(pathString.substr(0, count));
	}

	std::filesystem::path EditorAssetDatabase::GetFilesystemPath(const std::filesystem::path& relativePath) const
	{
		return m_ProjectDirectory / relativePath;
	}

	std::filesystem::path EditorAssetDatabase::GetRelativePath(const std::filesystem::path& filesystemPath) const
	{
		std::filesystem::path relativePath = filesystemPath.lexically_normal();
		if (filesystemPath.string().find(m_ProjectDirectory.string()) != std::string::npos)
		{
			relativePath = std::filesystem::relative(filesystemPath, m_ProjectDirectory);
			if (relativePath.empty())
				relativePath = filesystemPath.lexically_normal();
		}
		return relativePath;
	}

	std::filesystem::path EditorAssetDatabase::GetAvailableAssetPath(const std::filesystem::path& assetPath) const
	{
		std::filesystem::path result = assetPath;

		if (std::filesystem::exists(GetFilesystemPath(result)))
		{
			auto directoryPath = result.parent_path();

			bool found = false;
			uint32 count = 1;
			while (!found)
			{
				std::string nextPath = directoryPath.string() + "/" + result.stem().string();
				nextPath += " (";
				nextPath += std::to_string(count);
				nextPath += ")";
				nextPath += result.extension().string();

				if (!std::filesystem::exists(GetFilesystemPath(nextPath)))
				{
					found = true;
					result = nextPath;
					break;
				}

				count++;
			}
		}

		return result;
	}

	void EditorAssetDatabase::CreateFolder(const std::filesystem::path& parentFolder, const std::string& newFolderName)
	{
		std::filesystem::path path = GetAvailableAssetPath(parentFolder / newFolderName);
		std::filesystem::create_directories(GetFilesystemPath(path));
	}

}