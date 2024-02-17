#include "FluxPCH.h"
#include "RuntimeEngine.h"

#include "Flux/Runtime/Renderer/Renderer.h"

#include <glad/glad.h>

#include <stb_image.h>

namespace Flux {

	namespace TextureLoader	{

		static Ref<Texture> LoadTextureFromFile(const std::filesystem::path& path)
		{
			std::string pathString = path.string();

			int32 width;
			int32 height;
			void* data;

			TextureFormat format;
			if (stbi_is_hdr(pathString.c_str()))
			{
				data = stbi_loadf(pathString.c_str(), &width, &height, nullptr, STBI_rgb_alpha);
				FLUX_VERIFY(data, "{0}", stbi_failure_reason());
				format = TextureFormat::RGBAFloat;
			}
			else
			{
				data = stbi_load(pathString.c_str(), &width, &height, nullptr, STBI_rgb_alpha);
				FLUX_VERIFY(data, "{0}", stbi_failure_reason());
				format = TextureFormat::RGBA32;
			}

			TextureCreateInfo createInfo;
			createInfo.Width = width;
			createInfo.Height = height;
			createInfo.InitialData = data;
			createInfo.Format = format;

			Ref<Texture> texture = Texture::Create(createInfo);

			stbi_image_free(data);
			return texture;
		}
	}

	RuntimeEngine::RuntimeEngine(const EngineCreateInfo& createInfo)
		: Engine(createInfo)
	{
	}

	RuntimeEngine::~RuntimeEngine()
	{
		FLUX_CHECK_IS_IN_EVENT_THREAD();
	}

	void RuntimeEngine::OnInit()
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		uint32 width = m_MainWindow->GetWidth();
		uint32 height = m_MainWindow->GetHeight();
		m_EditorCamera.SetViewportSize(width, height);

		m_Shader = Shader::Create("Resources/Shaders/Shader.glsl");

		GraphicsPipelineCreateInfo pipelineCreateInfo;
		pipelineCreateInfo.VertexDeclaration = {
			{ "a_Position", VertexElementFormat::Float3 },
			{ "a_Normal", VertexElementFormat::Float3 },
			{ "a_Tangent", VertexElementFormat::Float3 },
			{ "a_Binormal", VertexElementFormat::Float3 },
			{ "a_TexCoord", VertexElementFormat::Float2 }
		};
		pipelineCreateInfo.DepthTest = true;
		pipelineCreateInfo.DepthWrite = true;
		pipelineCreateInfo.BackfaceCulling = true;
		m_Pipeline = GraphicsPipeline::Create(pipelineCreateInfo);

		m_SphereMesh = Mesh::LoadFromFile("Resources/Meshes/Sphere.glb");
	
		// m_CubemapTexture = TextureLoader::LoadTextureFromFile("Resources/Textures/newport_loft.hdr");

		{
			uint32 whiteTextureData = 0xFFFFFFFF;

			TextureCreateInfo textureCreateInfo;
			textureCreateInfo.Width = 1;
			textureCreateInfo.Height = 1;
			textureCreateInfo.InitialData = &whiteTextureData;
			textureCreateInfo.Format = TextureFormat::RGBA32;
			m_WhiteTexture = Texture::Create(textureCreateInfo);
		}

		{
			TextureCreateInfo textureCreateInfo;
			textureCreateInfo.Width = 16;
			textureCreateInfo.Height = 16;
			textureCreateInfo.Format = TextureFormat::RGBA32;
			m_CheckerboardTexture = Texture::Create(textureCreateInfo);

			for (uint32 y = 0; y < m_CheckerboardTexture->GetHeight(); y++)
			{
				for (uint32 x = 0; x < m_CheckerboardTexture->GetWidth(); x++)
				{
					uint32 color = (x + y % 2 == 0) ? 0xFFFFFFFF : 0xFF808080;
					m_CheckerboardTexture->SetPixel(x, y, color);
				}
			}
			m_CheckerboardTexture->Apply();
		}

		// Default material
		{
#if 0
			m_Material.AlbedoMap = TextureLoader::LoadTextureFromFile("Resources/Textures/rustediron2/rustediron2_basecolor.png");
			m_Material.NormalMap = TextureLoader::LoadTextureFromFile("Resources/Textures/rustediron2/rustediron2_normal.png");
			m_Material.RoughnessMap = TextureLoader::LoadTextureFromFile("Resources/Textures/rustediron2/rustediron2_roughness.png");
			m_Material.MetalnessMap = TextureLoader::LoadTextureFromFile("Resources/Textures/rustediron2/rustediron2_metallic.png");
#else
			m_Material.AlbedoMap = m_WhiteTexture;
			m_Material.NormalMap = m_WhiteTexture;
			m_Material.RoughnessMap = m_WhiteTexture;
			m_Material.MetalnessMap = m_WhiteTexture;
#endif

			m_Material.AlbedoColor = Vector4(1.0f);
			m_Material.Metalness = 0.0f;
			m_Material.Roughness = 0.5f;
			m_Material.Emission = 0.0f;
			m_Material.Name = "Default Material";
		}
	}

	void RuntimeEngine::OnShutdown()
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		m_Shader = nullptr;
		m_Pipeline = nullptr;

		m_SphereMesh = nullptr;

		m_Material = {};
		m_CubemapTexture = nullptr;
	}

	void RuntimeEngine::OnUpdate()
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		m_SwapchainFramebuffer->Bind();

		// TODO
		float deltaTime = m_DeltaTime;
		m_EditorCamera.OnUpdate(deltaTime);

		m_SphereMesh->GetVertexBuffer()->Bind();
		m_Pipeline->Bind();
		m_Pipeline->Scissor(0, 0, m_MainWindow->GetWidth(), m_MainWindow->GetHeight());
		m_SphereMesh->GetIndexBuffer()->Bind();

		Quaternion lightRotation = Quaternion(m_LightRotation * Math::DegToRad);
		Vector3 lightDirection = lightRotation * Vector3(0.0f, 0.0f, 1.0f);

		m_Shader->Bind();
		m_Shader->SetUniform("u_LightColor", m_LightColor);
		m_Shader->SetUniform("u_AmbientMultiplier", m_AmbientMultiplier);
		m_Shader->SetUniform("u_ViewMatrix", m_EditorCamera.GetViewMatrix());
		m_Shader->SetUniform("u_ViewProjectionMatrix", m_EditorCamera.GetProjectionMatrix() * m_EditorCamera.GetViewMatrix());
		m_Shader->SetUniform("u_CameraPosition", m_EditorCamera.GetPosition());
		m_Shader->SetUniform("u_LightDirection", lightDirection);

		uint32 numSpheresX = 7;
		uint32 numSpheresY = 7;
		float spacing = 2.5f;

		for (uint32 x = 0; x < numSpheresX; x++)
		{
			MaterialDescriptor material = m_Material;
			material.Roughness = Math::Clamp((float)x / (float)numSpheresX, 0.05f, 1.0f);

			for (uint32 y = 0; y < numSpheresY; y++)
			{
				material.Metalness = (float)y / (float)numSpheresY;
			
				Matrix4x4 transform = Math::BuildTransformationMatrix({ 
					(float)(x - ((float)numSpheresX * 0.5f)) * spacing,
					(float)(y - ((float)numSpheresY * 0.5f)) * spacing, 0.0f
				}, Vector3(0.0f));
				RenderMeshWithMaterial(m_SphereMesh, material, transform);
			}
		}

		m_SwapchainFramebuffer->Unbind();
	}

	void RuntimeEngine::RenderMesh(Ref<Mesh> mesh, const Matrix4x4& transform)
	{
		mesh->GetVertexBuffer()->Bind();
		m_Pipeline->Bind();
		m_Pipeline->Scissor(0, 0, m_MainWindow->GetWidth(), m_MainWindow->GetHeight());
		mesh->GetIndexBuffer()->Bind();

		m_Shader->Bind();

		auto& properties = m_SphereMesh->GetProperties();
		for (size_t i = 0; i < properties.Submeshes.size(); i++)
		{
			auto& submesh = properties.Submeshes[i];
			m_Shader->SetUniform("u_Transform", transform * submesh.WorldTransform);

			auto& material = properties.Materials[submesh.MaterialIndex];
			m_Shader->SetUniform("u_AlbedoColor", material.AlbedoColor);
			m_Shader->SetUniform("u_Roughness", material.Roughness);
			m_Shader->SetUniform("u_Metalness", material.Metalness);
			m_Shader->SetUniform("u_Emission", material.Emission);

			bool hasNormalMap = !material.NormalMap.Equals(m_WhiteTexture);
			m_Shader->SetUniform("u_HasNormalMap", uint32(hasNormalMap ? 1 : 0));

			if (material.AlbedoMap)
			{
				material.AlbedoMap->Bind(0);
				m_Shader->SetUniform("u_AlbedoMap", 0);
			}

			if (material.NormalMap)
			{
				material.NormalMap->Bind(1);
				m_Shader->SetUniform("u_NormalMap", 1);
			}

			if (material.RoughnessMap)
			{
				material.RoughnessMap->Bind(2);
				m_Shader->SetUniform("u_RoughnessMap", 2);
			}

			if (material.MetalnessMap)
			{
				material.MetalnessMap->Bind(3);
				m_Shader->SetUniform("u_MetalnessMap", 3);
			}

			m_Pipeline->DrawIndexed(
				submesh.IndexFormat,
				submesh.IndexCount,
				submesh.StartIndexLocation,
				submesh.BaseVertexLocation
			);

			if (material.AlbedoMap)
				material.AlbedoMap->Unbind(0);
			if (material.NormalMap)
				material.NormalMap->Unbind(1);
			if (material.RoughnessMap)
				material.RoughnessMap->Unbind(2);
			if (material.MetalnessMap)
				material.MetalnessMap->Unbind(3);
		}
	}

	void RuntimeEngine::RenderMeshWithMaterial(Ref<Mesh> mesh, const MaterialDescriptor& material, const Matrix4x4& transform)
	{
		mesh->GetVertexBuffer()->Bind();
		m_Pipeline->Bind();
		m_Pipeline->Scissor(0, 0, m_MainWindow->GetWidth(), m_MainWindow->GetHeight());
		mesh->GetIndexBuffer()->Bind();

		m_Shader->Bind();

		auto& properties = m_SphereMesh->GetProperties();
		for (size_t i = 0; i < properties.Submeshes.size(); i++)
		{
			auto& submesh = properties.Submeshes[i];
			m_Shader->SetUniform("u_Transform", transform * submesh.WorldTransform);

			m_Shader->SetUniform("u_AlbedoColor", material.AlbedoColor);
			m_Shader->SetUniform("u_Roughness", material.Roughness);
			m_Shader->SetUniform("u_Metalness", material.Metalness);
			m_Shader->SetUniform("u_Emission", material.Emission);

			bool hasNormalMap = !material.NormalMap.Equals(m_WhiteTexture);
			m_Shader->SetUniform("u_HasNormalMap", uint32(hasNormalMap ? 1 : 0));

			if (material.AlbedoMap)
			{
				material.AlbedoMap->Bind(0);
				m_Shader->SetUniform("u_AlbedoMap", 0);
			}

			if (material.NormalMap)
			{
				material.NormalMap->Bind(1);
				m_Shader->SetUniform("u_NormalMap", 1);
			}

			if (material.RoughnessMap)
			{
				material.RoughnessMap->Bind(2);
				m_Shader->SetUniform("u_RoughnessMap", 2);
			}

			if (material.MetalnessMap)
			{
				material.MetalnessMap->Bind(3);
				m_Shader->SetUniform("u_MetalnessMap", 3);
			}

			m_Pipeline->DrawIndexed(
				submesh.IndexFormat,
				submesh.IndexCount,
				submesh.StartIndexLocation,
				submesh.BaseVertexLocation
			);

			if (material.AlbedoMap)
				material.AlbedoMap->Unbind(0);
			if (material.NormalMap)
				material.NormalMap->Unbind(1);
			if (material.RoughnessMap)
				material.RoughnessMap->Unbind(2);
			if (material.MetalnessMap)
				material.MetalnessMap->Unbind(3);
		}
	}

	void RuntimeEngine::OnImGuiRender()
	{
		ImGui::Begin("Debug");

		ImGui::ColorEdit4("Albedo Color", m_Material.AlbedoColor.GetPointer());

		ImGui::DragFloat("Ambient Multiplier", &m_AmbientMultiplier, 0.001f, 0.0f, 1.0f);

		ImGui::DragFloat3("Light Rotation", m_LightRotation.GetPointer());
		Quaternion lightRotation = Quaternion(m_LightRotation * Math::DegToRad);
		Vector3 lightDirection = lightRotation * Vector3(0.0f, 0.0f, 1.0f);
		ImGui::Text("Light Direction: [%.2f, %.2f, %.2f]", lightDirection.X, lightDirection.Y, lightDirection.Z);

		ImGui::ColorEdit3("Light Color", m_LightColor.GetPointer());

		ImGui::Text("Camera Position: [%.2f, %.2f, %.2f]", m_EditorCamera.GetPosition().X, m_EditorCamera.GetPosition().Y, m_EditorCamera.GetPosition().Z);
		ImGui::Text("Camera Rotation: [%.2f, %.2f, %.2f]", m_EditorCamera.GetRotation().X, m_EditorCamera.GetRotation().Y, m_EditorCamera.GetRotation().Z);

		ImGui::Separator();

		BuildConfiguration buildConfig = Engine::GetBuildConfiguration();
		const char* buildConfigString = Utils::BuildConfigurationToString(buildConfig);
		ImGui::Text("%s build", buildConfigString);
		ImGui::Separator();

		ImGui::Text("Ticks per second: %d", m_TicksPerSecond);
		ImGui::Text("Events per second: %d", m_EventsPerSecond);
		ImGui::Separator();

		ImGui::Checkbox("V-Sync", &m_VSync);
		ImGui::Text("%d fps", m_FramesPerSecond);
		ImGui::Text("Delta Time: %.2fms", m_DeltaTime * 1000.0f);
		// ImGui::DragFloat("Fixed Delta Time", &m_FixedDeltaTime, 0.001f, 0.001f, 0.1f);
		ImGui::Text("Fixed Delta Time: %.2f", m_FixedDeltaTime);

		ImGui::Separator();
		ImGui::Text("Command queues: %d", Renderer::GetQueueCount());

		ImGui::BeginDisabled();
		bool multithreaded = m_RenderThread != nullptr;
		ImGui::Checkbox("Multithreaded", &multithreaded);
		ImGui::EndDisabled();

		if (m_RenderThread)
		{
			ImGui::Separator();
			ImGui::Text("Render Thread wait: %.2fms", m_RenderThreadWaitTime);
		}

		ImGui::End();
	}

	void RuntimeEngine::OnEvent(Event& event)
	{
		EventHandler handler(event);
		handler.Bind<WindowResizeEvent>(FLUX_BIND_CALLBACK(OnWindowResizeEvent, this));
	}

	void RuntimeEngine::OnWindowResizeEvent(WindowResizeEvent& event)
	{
		m_EditorCamera.SetViewportSize(event.GetWidth(), event.GetHeight());
	}

}