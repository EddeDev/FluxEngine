#pragma once

#include "AssetID.h"

namespace Flux {

	struct AssetMetadata
	{
		AssetID ID;
		std::string Name;
		AssetType Type = AssetType::None;
		
		std::filesystem::path RelativeMetaPath;
		std::filesystem::path RelativeAssetPath;
		std::filesystem::path FilesystemMetaPath;
		std::filesystem::path FilesystemAssetPath;
		
		bool IsLoaded = false;
		bool IsMemoryAsset = false;

		operator bool() const { return ID && !IsMemoryAsset; }
	};

	static const char* s_AssetMetadataFileExtension = ".fluxmeta";

}