#pragma once

#include "Asset.h"
#include "AssetMetadata.h"

#include <filesystem>

namespace Flux {

	class AssetDatabase : public ReferenceCounted
	{
	public:
		virtual ~AssetDatabase() {}

		virtual Ref<Asset> ImportAsset(const Guid& assetID) = 0;
		virtual Ref<Asset> GetAssetFromID(const Guid& assetID) = 0;
		virtual void ImportAssets() = 0;
		virtual bool SaveAsset(const Ref<Asset>& asset) = 0;
		virtual void SaveAssets() = 0;

		virtual const AssetMetadata& GetMetadataFromPath(const std::filesystem::path& metadataPath) const = 0;
		virtual const AssetMetadata& GetMetadataFromAssetPath(const std::filesystem::path& assetPath) const = 0;
		virtual const AssetMetadata& GetMetadataFromAsset(const Ref<Asset>& asset) const = 0;
		virtual const AssetMetadata& GetMetadataFromAssetID(const Guid& assetID) const = 0;

		virtual std::filesystem::path GetMetadataPath(const std::filesystem::path& assetPath) const = 0;
		virtual std::filesystem::path GetAssetPath(const std::filesystem::path& metadataPath) const = 0;
		virtual std::filesystem::path GetFilesystemPath(const std::filesystem::path& relativePath) const = 0;
		virtual std::filesystem::path GetRelativePath(const std::filesystem::path& filesystemPath) const = 0;

		virtual std::unordered_map<Guid, Ref<Asset>>& GetAssetMap() = 0;
		virtual const std::unordered_map<Guid, Ref<Asset>>& GetAssetMap() const = 0;

		virtual std::unordered_map<Guid, Ref<Asset>>& GetMemoryAssetMap() = 0;
		virtual const std::unordered_map<Guid, Ref<Asset>>& GetMemoryAssetMap() const = 0;

		virtual std::unordered_map<Guid, AssetMetadata>& GetMetadataMap() = 0;
		virtual const std::unordered_map<Guid, AssetMetadata>& GetMetadataMap() const = 0;

		template<typename T = Asset>
		Ref<T> GetAssetFromID(const Guid& assetID)
		{
			static_assert(std::is_base_of<Asset, T>::value);
			return GetAssetFromID(assetID).As<T>();
		}

		template<typename T = Asset>
		Ref<T> GetAssetFromMetadata(const AssetMetadata& metadata)
		{
			static_assert(std::is_base_of<Asset, T>::value);
			return GetAssetFromID<T>(metadata.ID);
		}

		template<typename T = Asset>
		Ref<T> GetAssetFromPath(const std::filesystem::path& assetPath)
		{
			static_assert(std::is_base_of<Asset, T>::value);
			return GetAssetFromMetadata<T>(GetMetadataFromAssetPath(assetPath));
		}

		bool IsMemoryAsset(const Guid& assetID) const
		{
			auto& memoryAssetMap = GetMemoryAssetMap();
			return memoryAssetMap.find(assetID) != memoryAssetMap.end();
		}

		bool IsMemoryAsset(const Ref<Asset>& asset) const
		{
			return IsMemoryAsset(asset->GetID());
		}

		const std::string& GetAssetName(const Ref<Asset>& asset) const
		{
			return GetMetadataFromAsset(asset).Name;
		}
	};

}