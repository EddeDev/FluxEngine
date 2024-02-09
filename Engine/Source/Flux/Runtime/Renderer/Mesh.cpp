#include "FluxPCH.h"
#include "Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>

namespace Flux {

	static const uint32 s_AssimpImportFlags =
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_SortByPType |
		aiProcess_GenNormals |
		aiProcess_GenUVCoords |
		aiProcess_OptimizeMeshes | /*Optional?*/
		aiProcess_JoinIdenticalVertices |
		aiProcess_GlobalScale |
		aiProcess_ConvertToLeftHanded |
		aiProcess_ValidateDataStructure;

	Mesh::Mesh(const MeshProperties& properties)
		: m_Properties(properties)
	{
		m_VertexBuffer = VertexBuffer::Create(properties.Vertices.data(), properties.Vertices.size() * sizeof(Vertex));
		m_IndexBuffer = IndexBuffer::Create(properties.Indices.data(), properties.Indices.size());
	}

	static void LoadMeshNode(const aiScene* scene, const aiNode* node, MeshProperties& properties, const Matrix4x4& parentTransform = Matrix4x4(1.0f))
	{
		Matrix4x4 localTransform = Matrix4x4::Transpose(*(Matrix4x4*)&node->mTransformation.a1);
		Matrix4x4 worldTransform = parentTransform * localTransform;

		for (uint32 i = 0; i < node->mNumMeshes; i++)
		{
			uint32 meshIndex = node->mMeshes[i];

			const aiMesh* mesh = scene->mMeshes[meshIndex];
			if (!mesh)
				continue;

			SubmeshDescriptor& submesh = properties.Submeshes.emplace_back();
			submesh.Name = mesh->mName.C_Str();

			uint32 maxIndexValue = 0;
			for (uint32 faceIndex = 0; faceIndex < mesh->mNumFaces; faceIndex++)
			{
				const aiFace& face = mesh->mFaces[faceIndex];

				maxIndexValue = Math::Max(face.mIndices[0], maxIndexValue);
				maxIndexValue = Math::Max(face.mIndices[1], maxIndexValue);
				maxIndexValue = Math::Max(face.mIndices[2], maxIndexValue);
			}

			if (maxIndexValue <= std::numeric_limits<uint8>::max())
				submesh.IndexFormat = IndexFormat::UInt8;
			else if (maxIndexValue <= std::numeric_limits<uint16>::max())
				submesh.IndexFormat = IndexFormat::UInt16;
			else
				submesh.IndexFormat = IndexFormat::UInt32;

			submesh.BaseVertexLocation = (uint32)properties.Vertices.size();
			submesh.StartIndexLocation = (uint32)properties.Indices.size();
			submesh.VertexCount = mesh->mNumVertices;
			submesh.IndexCount = mesh->mNumFaces * 3;
			submesh.MaterialIndex = mesh->mMaterialIndex;

			submesh.LocalTransform = localTransform;
			submesh.WorldTransform = worldTransform;

			for (uint32 vertexIndex = 0; vertexIndex < mesh->mNumVertices; vertexIndex++)
			{
				Vertex& vertex = properties.Vertices.emplace_back();

				vertex.Position.X = mesh->mVertices[vertexIndex].x;
				vertex.Position.Y = mesh->mVertices[vertexIndex].y;
				vertex.Position.Z = mesh->mVertices[vertexIndex].z;

				if (mesh->HasNormals())
				{
					vertex.Normal.X = mesh->mNormals[vertexIndex].x;
					vertex.Normal.Y = mesh->mNormals[vertexIndex].y;
					vertex.Normal.Z = mesh->mNormals[vertexIndex].z;
				}

				if (mesh->HasTangentsAndBitangents())
				{
					vertex.Tangent.X = mesh->mTangents[vertexIndex].x;
					vertex.Tangent.Y = mesh->mTangents[vertexIndex].y;
					vertex.Tangent.Z = mesh->mTangents[vertexIndex].z;

					vertex.Binormal.X = mesh->mBitangents[vertexIndex].x;
					vertex.Binormal.Y = mesh->mBitangents[vertexIndex].y;
					vertex.Binormal.Z = mesh->mBitangents[vertexIndex].z;
				}

				if (mesh->HasTextureCoords(0))
				{
					vertex.TexCoord.X = mesh->mTextureCoords[0][vertexIndex].x;
					vertex.TexCoord.Y = mesh->mTextureCoords[0][vertexIndex].y;
				}
			}

			switch (submesh.IndexFormat)
			{
			case IndexFormat::UInt8:
			{
				for (uint32 faceIndex = 0; faceIndex < mesh->mNumFaces; faceIndex++)
				{
					const aiFace& face = mesh->mFaces[faceIndex];
					for (uint32 j = 0; j < face.mNumIndices; j++)
						properties.Indices.push_back((uint8)face.mIndices[j]);
				}
				break;
			}
			case IndexFormat::UInt16:
			{
				for (uint32 faceIndex = 0; faceIndex < mesh->mNumFaces; faceIndex++)
				{
					const aiFace& face = mesh->mFaces[faceIndex];
					for (uint32 j = 0; j < face.mNumIndices; j++)
					{
						uint8 index[2];
						*(uint16*)&index = (uint16)face.mIndices[j];

						properties.Indices.push_back(index[0]);
						properties.Indices.push_back(index[1]);
					}
				}
				break;
			}
			case IndexFormat::UInt32:
			{
				for (uint32 faceIndex = 0; faceIndex < mesh->mNumFaces; faceIndex++)
				{
					const aiFace& face = mesh->mFaces[faceIndex];
					for (uint32 j = 0; j < face.mNumIndices; j++)
					{
						uint8 index[4];
						*(uint32*)&index = (uint32)face.mIndices[j];

						properties.Indices.push_back(index[0]);
						properties.Indices.push_back(index[1]);
						properties.Indices.push_back(index[2]);
						properties.Indices.push_back(index[3]);
					}
				}
				break;
			}
			}
		}

		for (uint32 i = 0; i < node->mNumChildren; i++)
			LoadMeshNode(scene, node->mChildren[i], properties, worldTransform);
	}

	Ref<Mesh> Mesh::LoadFromFile(const std::filesystem::path& path)
	{
		MeshProperties properties;

		Assimp::Importer importer;

		const aiScene* scene = importer.ReadFile(path.string(), s_AssimpImportFlags);
		if (!scene)
		{
			FLUX_ERROR_CATEGORY("Assimp", "{0}", importer.GetErrorString());
			return nullptr;
		}

		LoadMeshNode(scene, scene->mRootNode, properties);

		return Ref<Mesh>::Create(properties);
	}

}