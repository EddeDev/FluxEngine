#pragma once

namespace Flux {

	class AssetImporter
	{
	};

	enum class ModelNormalImportMode : uint8
	{
		None = 0,
		Import,
		Calculate,
	};

	enum class ModelTangentImportMode : uint8
	{
		None = 0,
		Import,
		Calculate,
	};

	enum class ModelMaterialImportMode : uint8
	{
		None = 0,
		Import
	};

	struct ModelImportSettings
	{
		// Model
		float ScaleFactor = 1.0f;
		bool ImportVisibility = true;
		bool ImportCameras = true;
		bool ImportLights = true;
		ModelNormalImportMode NormalImportMode = ModelNormalImportMode::Import;
		ModelTangentImportMode TangentImportMode = ModelTangentImportMode::Calculate;
		bool OptimizeMeshes;
		bool JoinIdenticalVertices;

		// Materials
		ModelMaterialImportMode MaterialImportMode = ModelMaterialImportMode::Import;
	};

	class ModelImporter : public AssetImporter
	{
	private:
		ModelImportSettings m_ImportSettings;
	};

}