#pragma once

#include "Flux/Runtime/Core/BaseTypes.h"
#include "Flux/Runtime/Core/AssertionMacros.h"

#include <string>
#include <unordered_map>
#include <filesystem>

namespace Flux {

	enum class AssetType : uint8
	{
		None = 0,

		Scene,
		Mesh,
		Material,
		Texture
	};

	namespace Utils {

		inline static std::unordered_map<std::string_view, AssetType> s_AssetExtensionMap =
		{
			{ ".scene", AssetType::Scene },
			{ ".mat",   AssetType::Material },
					    
			{ ".fbx",   AssetType::Mesh },
			{ ".dae",   AssetType::Mesh },
			{ ".gltf",  AssetType::Mesh },
			{ ".glb",   AssetType::Mesh },
			{ ".blend", AssetType::Mesh },
			{ ".3ds",   AssetType::Mesh },
			{ ".obj",   AssetType::Mesh },
			{ ".ply",   AssetType::Mesh },
			{ ".stl",   AssetType::Mesh },
					    
			{ ".png",   AssetType::Texture },
			{ ".jpg",   AssetType::Texture },
			{ ".jpeg",  AssetType::Texture },
			{ ".bmp",   AssetType::Texture },
			{ ".psd",   AssetType::Texture },
			{ ".tga",   AssetType::Texture },
			{ ".gif",   AssetType::Texture },
			{ ".hdr",   AssetType::Texture }
		};

		inline static AssetType AssetTypeFromExtension(const std::string& extension)
		{
			FLUX_VERIFY(extension[0] == '.');

			if (s_AssetExtensionMap.find(extension) != s_AssetExtensionMap.end())
				return s_AssetExtensionMap.at(extension);

			return AssetType::None;
		}

		inline static AssetType AssetTypeFromPath(const std::filesystem::path& path)
		{
			std::string extension = path.extension().string();
			if (!extension.empty())
				return AssetTypeFromExtension(path.extension().string());

			return AssetType::None;
		}

		inline static AssetType AssetTypeFromString(std::string_view name)
		{
			if (name == "None")     return AssetType::None;
			if (name == "Scene")    return AssetType::Scene;
			if (name == "Mesh")     return AssetType::Mesh;
			if (name == "Material") return AssetType::Material;
			if (name == "Texture")  return AssetType::Texture;
			FLUX_VERIFY(false);
			return AssetType::None;
		}

		inline static const char* AssetTypeToString(AssetType type)
		{
			switch (type)
			{
			case AssetType::None:     return "None";
			case AssetType::Scene:    return "Scene";
			case AssetType::Mesh:     return "Mesh";
			case AssetType::Material: return "Material";
			case AssetType::Texture:  return "Texture";
			}
			FLUX_VERIFY(false, "Unknown asset type");
			return "";
		}

	}

}