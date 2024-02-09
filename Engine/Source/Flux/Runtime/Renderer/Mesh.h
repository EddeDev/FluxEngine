#pragma once

#include "VertexBuffer.h"
#include "IndexBuffer.h"

#include "GraphicsPipeline.h"

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

	struct MeshProperties
	{
		std::vector<Vertex> Vertices;
		std::vector<uint8> Indices;
		std::vector<SubmeshDescriptor> Submeshes;
	};

	class Mesh : public ReferenceCounted
	{
	public:
		Mesh(const MeshProperties& properties);

		Ref<VertexBuffer> GetVertexBuffer() const { return m_VertexBuffer; }
		Ref<IndexBuffer> GetIndexBuffer() const { return m_IndexBuffer; }

		const MeshProperties& GetProperties() const { return m_Properties; }

		static Ref<Mesh> LoadFromFile(const std::filesystem::path& path);
	private:
		MeshProperties m_Properties;

		Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer> m_IndexBuffer;
	};

}