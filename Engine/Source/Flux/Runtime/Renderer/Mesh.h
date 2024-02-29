#pragma once

#include "Flux/Runtime/Asset/Asset.h"

#include "VertexBuffer.h"
#include "IndexBuffer.h"

#include "GraphicsPipeline.h"

#include "Texture.h"

namespace Flux {

	struct Vertex
	{
		Vector3 Position;
		Vector3 Normal;
		Vector3 Tangent;
		Vector3 Binormal;
		Vector2 TexCoord;
	};

	struct SubmeshDescriptor
	{
		uint32 BaseVertexLocation;
		uint32 StartIndexLocation;
		uint32 VertexCount;
		uint32 IndexCount;
		uint32 MaterialIndex;

		Flux::IndexFormat IndexFormat;

		Matrix4x4 WorldTransform;
		Matrix4x4 LocalTransform;

		std::string Name;
	};

	struct MaterialDescriptor
	{
		Vector4 AlbedoColor;
		float Metalness;
		float Roughness;
		float Emission;

		Ref<Texture> AlbedoMap;
		Ref<Texture> NormalMap;
		Ref<Texture> RoughnessMap;
		Ref<Texture> MetalnessMap;

		std::string Name;
	};

	struct MeshProperties
	{
		std::vector<Vertex> Vertices;
		std::vector<uint8> Indices;
		std::vector<SubmeshDescriptor> Submeshes;
		std::vector<MaterialDescriptor> Materials;
	};

	class Mesh : public Asset
	{
	public:
		Mesh(const MeshProperties& properties);

		Ref<VertexBuffer> GetVertexBuffer() const { return m_VertexBuffer; }
		Ref<IndexBuffer> GetIndexBuffer() const { return m_IndexBuffer; }

		const MeshProperties& GetProperties() const { return m_Properties; }

		static Ref<Mesh> LoadFromFile(const std::filesystem::path& path);

		ASSET_CLASS_TYPE(Mesh)
	private:
		MeshProperties m_Properties;

		Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer> m_IndexBuffer;
	};

}