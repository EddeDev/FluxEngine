#include "FluxPCH.h"
#include "EditorAssetDatabase.h"

#include "Flux/Runtime/Utils/FileHelper.h"




// TODO: TEMP
#include "Flux/Runtime/Renderer/Mesh.h"

#include <yaml-cpp/yaml.h>

namespace Flux {

	static AssetMetadata s_NullMetadata;
	static AssetID s_NullAssetID;

	EditorAssetDatabase::EditorAssetDatabase(const std::filesystem::path& projectDirectory, const std::filesystem::path& assetDirectory)
		: m_ProjectDirectory(projectDirectory), m_AssetDirectory(assetDirectory)
	{
		Refresh();
	}

	EditorAssetDatabase::~EditorAssetDatabase()
	{
	}

	Ref<Asset> EditorAssetDatabase::ImportAsset(const AssetID& assetID)
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

#if TODO_WIP
			metadataIt->second.IsLoaded = AssetImporter::Import(metadata, asset);
#else
			if (metadataIt->second.Type == AssetType::Mesh)
			{
				asset = Mesh::LoadFromFile(metadataIt->second.FilesystemAssetPath);
				asset->SetAssetID(assetID);
				metadataIt->second.IsLoaded = true;
			}
#endif
			

			if (!metadataIt->second.IsLoaded)
				return nullptr;

			m_AssetMap[assetID] = asset;
		}

		return m_AssetMap[assetID];
	}

	Ref<Asset> EditorAssetDatabase::GetAssetFromID(const AssetID& assetID)
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

		auto& assetID = asset->GetAssetID();

		auto it = m_MetadataMap.find(assetID);
		if (it == m_MetadataMap.end())
			return false;

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

	const AssetID& EditorAssetDatabase::CreateMetadata(const std::filesystem::path& metadataPath)
	{
		auto assetPath = GetAssetPath(metadataPath);

		AssetID assetID = Utils::GenerateAssetID();

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

	const AssetID& EditorAssetDatabase::ImportMetadata(const std::filesystem::path& metadataPath)
	{
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(GetFilesystemPath(metadataPath).string());
		}
		catch (YAML::Exception e)
		{
			FLUX_VERIFY(false, "{0}", e.msg);
		}

		AssetID assetID;
		Guid::Parse(data["GUID"].as<std::string>(), assetID);

		AssetMetadata& metadata = m_MetadataMap[assetID];
		metadata.ID = assetID;
		metadata.Name = GetAssetPath(metadataPath).filename().stem().string();
		metadata.Type = Utils::AssetTypeFromString(data["Type"].as<std::string>());
		metadata.RelativeMetaPath = metadataPath;
		metadata.FilesystemMetaPath = GetFilesystemPath(metadata.RelativeMetaPath);
		metadata.RelativeAssetPath = GetAssetPath(metadataPath);
		metadata.FilesystemAssetPath = GetFilesystemPath(metadata.RelativeAssetPath);
		return metadata.ID;
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

		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "GUID" << YAML::Value << metadata.ID.ToString();
		out << YAML::Key << "Type" << YAML::Value << Utils::AssetTypeToString(metadata.Type);
		out << YAML::EndMap;
		return FileHelper::SaveStringToFile(out.c_str(), GetFilesystemPath(metadataPath));
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
		auto& assetID = asset->GetAssetID();
		if (m_MetadataMap.find(assetID) != m_MetadataMap.end())
			return m_MetadataMap.at(assetID);
		return s_NullMetadata;
	}

	const AssetMetadata& EditorAssetDatabase::GetMetadataFromAssetID(const AssetID& assetID) const
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

	bool EditorAssetDatabase::CreateFolder(const std::filesystem::path& parentFolder, const std::string& newFolderName) const
	{
		std::filesystem::path path = GetAvailableAssetPath(parentFolder / newFolderName);
		return std::filesystem::create_directories(GetFilesystemPath(path));
	}

}