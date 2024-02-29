#pragma once

#include "Asset.h"

#include "Flux/Editor/Project/Project.h"

namespace Flux {

	class AssetDatabase
	{
	public:
		static Ref<Asset> ImportAsset(const AssetID& assetID)
		{
			return Project::GetActive()->GetAssetDatabase()->ImportAsset(assetID);
		}

		static Ref<Asset> GetAssetFromID(const AssetID& assetID)
		{
			return Project::GetActive()->GetAssetDatabase()->GetAssetFromID(assetID);
		}

		static const AssetMetadata& GetMetadataFromPath(const std::filesystem::path& metadataPath)
		{
			return Project::GetActive()->GetAssetDatabase()->GetMetadataFromPath(metadataPath);
		}

		static const AssetMetadata& GetMetadataFromAssetPath(const std::filesystem::path& assetPath)
		{
			return Project::GetActive()->GetAssetDatabase()->GetMetadataFromAssetPath(assetPath);
		}

		static const AssetMetadata& GetMetadataFromAsset(const Ref<Asset>& asset)
		{
			return Project::GetActive()->GetAssetDatabase()->GetMetadataFromAsset(asset);
		}

		static const AssetMetadata& GetMetadataFromAssetID(const AssetID& assetID)
		{
			return Project::GetActive()->GetAssetDatabase()->GetMetadataFromAssetID(assetID);
		}

		static std::filesystem::path GetMetadataPath(const std::filesystem::path& assetPath)
		{
			return Project::GetActive()->GetAssetDatabase()->GetMetadataPath(assetPath);
		}

		static std::filesystem::path GetAssetPath(const std::filesystem::path& metadataPath)
		{
			return Project::GetActive()->GetAssetDatabase()->GetAssetPath(metadataPath);
		}

		static std::filesystem::path GetFilesystemPath(const std::filesystem::path& relativePath)
		{
			return Project::GetActive()->GetAssetDatabase()->GetFilesystemPath(relativePath);
		}

		static std::filesystem::path GetRelativePath(const std::filesystem::path& filesystemPath)
		{
			return Project::GetActive()->GetAssetDatabase()->GetRelativePath(filesystemPath);
		}

		template<typename T = Asset>
		static Ref<T> GetAssetFromID(const AssetID& assetID)
		{
			return Project::GetActive()->GetAssetDatabase()->GetAssetFromID<T>(assetID);
		}

		template<typename T = Asset>
		static Ref<T> GetAssetFromMetadata(const AssetMetadata& metadata)
		{
			return Project::GetActive()->GetAssetDatabase()->GetAssetFromMetadata<T>(metadata);
		}

		template<typename T = Asset>
		static Ref<T> GetAssetFromPath(const std::filesystem::path& assetPath)
		{
			return Project::GetActive()->GetAssetDatabase()->GetAssetFromPath<T>(assetPath);
		}

		static bool IsMemoryAsset(const AssetID& assetID)
		{
			return Project::GetActive()->GetAssetDatabase()->IsMemoryAsset(assetID);
		}

		static bool IsMemoryAsset(const Ref<Asset>& asset)
		{
			return Project::GetActive()->GetAssetDatabase()->IsMemoryAsset(asset);
		}

		static const std::string& GetAssetName(const Ref<Asset>& asset)
		{
			return Project::GetActive()->GetAssetDatabase()->GetAssetName(asset);
		}
	};
}