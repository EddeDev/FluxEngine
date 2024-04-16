#pragma once

#include "Flux/Runtime/Asset/AssetDatabaseInterface.h"

namespace Flux {

	class EditorAssetDatabase : public AssetDatabaseInterface
	{
	public:
		EditorAssetDatabase(const std::filesystem::path& projectDirectory, const std::filesystem::path& assetDirectory);
		virtual ~EditorAssetDatabase();

		virtual Ref<Asset> ImportAsset(const AssetID& assetID) override;
		virtual Ref<Asset> GetAssetFromID(const AssetID& assetID) override;
		virtual void ImportAssets() override;
		virtual bool SaveAsset(const Ref<Asset>& asset) override;
		virtual void SaveAssets() override;

		virtual const AssetMetadata& GetMetadataFromPath(const std::filesystem::path& metadataPath) const override;
		virtual const AssetMetadata& GetMetadataFromAssetPath(const std::filesystem::path& assetPath) const override;
		virtual const AssetMetadata& GetMetadataFromAsset(const Ref<Asset>& asset) const override;
		virtual const AssetMetadata& GetMetadataFromAssetID(const AssetID& assetID) const override;

		virtual std::filesystem::path GetMetadataPath(const std::filesystem::path& assetPath) const override;
		virtual std::filesystem::path GetAssetPath(const std::filesystem::path& metadataPath) const override;
		virtual std::filesystem::path GetFilesystemPath(const std::filesystem::path& relativePath) const override;
		virtual std::filesystem::path GetRelativePath(const std::filesystem::path& filesystemPath) const override;

		virtual std::unordered_map<AssetID, Ref<Asset>>& GetAssetMap() override { return m_AssetMap; }
		virtual const std::unordered_map<AssetID, Ref<Asset>>& GetAssetMap() const override { return m_AssetMap; }

		virtual std::unordered_map<AssetID, Ref<Asset>>& GetMemoryAssetMap() override { return m_MemoryAssetMap; }
		virtual const std::unordered_map<AssetID, Ref<Asset>>& GetMemoryAssetMap() const override { return m_MemoryAssetMap; }

		virtual std::unordered_map<AssetID, AssetMetadata>& GetMetadataMap() override { return m_MetadataMap; }
		virtual const std::unordered_map<AssetID, AssetMetadata>& GetMetadataMap() const override { return m_MetadataMap; }

		void Refresh();

		std::filesystem::path GetAvailableAssetPath(const std::filesystem::path& assetPath) const;
		bool CreateFolder(const std::filesystem::path& parentFolder, const std::string& newFolderName) const;

		template<typename T, typename... TArgs>
		Ref<T> CreateAsset(std::filesystem::path assetPath, TArgs&&... args)
		{
			assetPath = GetAvailableAssetPath(assetPath);

			auto& assetID = CreateMetadata(GetMetadataPath(assetPath));
			Ref<Asset> asset = Ref<T>::Create(std::forward<TArgs>(args)...);
			asset->SetAssetID(assetID);

			m_AssetMap[assetID] = asset;
			SaveAsset(asset);
			return asset;
		}

		template<typename T, typename... TArgs>
		Ref<T> CreateMemoryAsset(TArgs&&... args)
		{
			AssetMetadata metadata;
			metadata.ID = Utils::GenerateAssetID();
			metadata.Type = T::GetStaticType();
			metadata.Name = fmt::format("Untitled {0}", Utils::AssetTypeToString(metadata.Type));
			metadata.IsMemoryAsset = true;

			Ref<Asset> asset = Ref<T>::Create(std::forward<TArgs>(args)...);
			asset->SetAssetID(metadata.ID);

			m_MemoryAssetMap[metadata.ID] = asset;
			m_MetadataMap[metadata.ID] = metadata;
			return asset;
		}

		template<typename T, typename... TArgs>
		Ref<T> CreateAssetFromMemoryAsset(const Ref<T>& asset, std::filesystem::path assetPath, TArgs&&... args)
		{
			FLUX_VERIFY(IsMemoryAsset(asset));

			assetPath = GetAvailableAssetPath(assetPath);

			auto& metadata = m_MetadataMap.at(asset->ID);
			metadata.Name = assetPath.filename().stem().string();
			metadata.RelativeAssetPath = assetPath;
			metadata.RelativeMetaPath = GetMetadataPath(assetPath);
			metadata.FilesystemAssetPath = GetFilesystemPath(metadata.RelativeAssetPath);
			metadata.FilesystemMetaPath = GetFilesystemPath(metadata.RelativeMetaPath);
			metadata.IsLoaded = true;
			metadata.IsMemoryAsset = false;

			SaveMetadata(metadata.RelativeMetaPath);
			m_AssetMap[metadata.ID] = m_MemoryAssetMap.at(metadata.ID);
			m_MemoryAssetMap.erase(metadata.ID);
			return asset;
		}
	private:
		const AssetID& CreateMetadata(const std::filesystem::path& metadataPath);
		const AssetID& ImportMetadata(const std::filesystem::path& metadataPath);
		bool RemoveMetadata(const std::filesystem::path& metadataPath);
		bool SaveMetadata(const std::filesystem::path& metadataPath) const;
	private:
		std::filesystem::path m_ProjectDirectory;
		std::filesystem::path m_AssetDirectory;

		std::unordered_map<AssetID, AssetMetadata> m_MetadataMap;
		std::unordered_map<AssetID, Ref<Asset>> m_AssetMap;
		std::unordered_map<AssetID, Ref<Asset>> m_MemoryAssetMap;
	};

}