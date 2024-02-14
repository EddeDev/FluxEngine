#include "FluxPCH.h"
#include "RuntimeEngine.h"

#include "Flux/Runtime/Renderer/Renderer.h"

#include <glad/glad.h>

namespace Flux {

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
		m_EditorCamera.SetPosition({ 5.0f, 4.0f, -8.0f });
		m_EditorCamera.SetRotation({ 20.0f, -25.0f, 0.0f });

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

		m_GunMesh = Mesh::LoadFromFile("Resources/Meshes/makarov_pm/scene.gltf");
		m_CityMesh = Mesh::LoadFromFile("Resources/Meshes/low_poly_city/scene.gltf");
		m_BackpackMesh = Mesh::LoadFromFile("Resources/Meshes/backpack/backpack.obj");

		m_ActiveMesh = m_GunMesh;
		m_NumVisibleSubmeshes = m_ActiveMesh->GetProperties().Submeshes.size();
	}

	void RuntimeEngine::OnShutdown()
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		m_Shader = nullptr;
		m_Pipeline = nullptr;

		m_GunMesh = nullptr;
		m_CityMesh = nullptr;
		m_BackpackMesh = nullptr;

		m_ActiveMesh = nullptr;
	}

	static int32 s_NumMeshesX = 1;
	static int32 s_NumMeshesZ = 1;
	static float s_MeshSpacing = 0.0f;

	void RuntimeEngine::OnUpdate()
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		m_SwapchainFramebuffer->Bind();

		// TODO
		float deltaTime = m_DeltaTime;
		m_EditorCamera.OnUpdate(deltaTime);

		if (m_ActiveMesh)
		{
			m_ActiveMesh->GetVertexBuffer()->Bind();
			m_Pipeline->Bind();
			m_Pipeline->Scissor(0, 0, m_MainWindow->GetWidth(), m_MainWindow->GetHeight());
			m_ActiveMesh->GetIndexBuffer()->Bind();

			m_Shader->Bind();
			m_Shader->SetUniform("u_LightColor", m_LightColor);
			m_Shader->SetUniform("u_LightPosition", m_LightPosition);
			m_Shader->SetUniform("u_AmbientMultiplier", m_AmbientMultiplier);
			m_Shader->SetUniform("u_ViewProjectionMatrix", m_EditorCamera.GetProjectionMatrix() * m_EditorCamera.GetViewMatrix());

			auto& properties = m_ActiveMesh->GetProperties();

			for (int32 x = 0; x < s_NumMeshesX; x++)
			{
				for (int32 z = 0; z < s_NumMeshesZ; z++)
				{
					Matrix4x4 meshTransform = Math::BuildTransformationMatrix({ (x - s_NumMeshesX / 2) * s_MeshSpacing, 0.0f, (z - s_NumMeshesZ / 2) * s_MeshSpacing }, Vector3(0.0f), Vector3(1.0f));

					for (uint32 i = 0; i < m_NumVisibleSubmeshes; i++)
					{
						auto& submesh = properties.Submeshes[i];
						m_Shader->SetUniform("u_Transform", meshTransform * submesh.WorldTransform);

						auto& material = properties.Materials[submesh.MaterialIndex];
						m_Shader->SetUniform("u_AlbedoColor", material.AlbedoColor);
						m_Shader->SetUniform("u_Roughness", material.Roughness);
						m_Shader->SetUniform("u_Metalness", material.Metalness);
						m_Shader->SetUniform("u_Emission", material.Emission);

						m_Shader->SetUniform("u_HasNormalMap", uint32((material.NormalMap && m_UseNormalMaps) ? 1 : 0));

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
					}
				}
			}
		}

		m_SwapchainFramebuffer->Unbind();
	}

	void RuntimeEngine::OnImGuiRender()
	{
		ImGui::Begin("Debug");

		static const char* s_MeshStrings[]{
			"Gun",
			"City",
			"Backpack"
		};

		int32 selectedMeshIndex;
		if (m_ActiveMesh == m_GunMesh)
			selectedMeshIndex = 0;
		else if (m_ActiveMesh == m_CityMesh)
			selectedMeshIndex = 1;
		else if (m_ActiveMesh == m_BackpackMesh)
			selectedMeshIndex = 2;
		else
			FLUX_VERIFY(false);

		if (ImGui::Combo("Active Mesh", &selectedMeshIndex, s_MeshStrings, IM_ARRAYSIZE(s_MeshStrings)))
		{
			if (selectedMeshIndex == 0)
				m_ActiveMesh = m_GunMesh;
			else if (selectedMeshIndex == 1)
				m_ActiveMesh = m_CityMesh;
			else if (selectedMeshIndex == 2)
				m_ActiveMesh = m_BackpackMesh;
			else
				FLUX_VERIFY(false);

			m_NumVisibleSubmeshes = m_ActiveMesh->GetProperties().Submeshes.size();
		}

		ImGui::Text("Submesh Count: %d", m_ActiveMesh->GetProperties().Submeshes.size());
		ImGui::DragInt("Submeshes", &m_NumVisibleSubmeshes, 0.1f, 0, m_ActiveMesh->GetProperties().Submeshes.size());

		m_NumVisibleSubmeshes = Math::Clamp(m_NumVisibleSubmeshes, 0, (int32)m_ActiveMesh->GetProperties().Submeshes.size());

		int32 lastVisibleIndex = (int32)m_NumVisibleSubmeshes - 1;
		if (lastVisibleIndex >= 0)
		{
			auto& submesh = m_ActiveMesh->GetProperties().Submeshes.at(lastVisibleIndex);
			ImGui::Text("Last Visible Index (%d):", lastVisibleIndex);
			ImGui::Text("  Base Vertex Location: %d", submesh.BaseVertexLocation);
			ImGui::Text("  Start Index Location: %d", submesh.StartIndexLocation);
			ImGui::Text("  Vertex Count: %d", submesh.VertexCount);
			ImGui::Text("  Index Count: %d", submesh.IndexCount);
			ImGui::Text("  Index Format: %s", Utils::IndexFormatToString(submesh.IndexFormat));
			ImGui::Text("  Mesh Name: %s", submesh.Name.c_str());
			ImGui::Text("  Material Index: %d", submesh.MaterialIndex);
			ImGui::Text("  Has Normals: %s", m_ActiveMesh->GetProperties().Materials[submesh.MaterialIndex].NormalMap ? "true" : "false");
		}

		ImGui::Separator();

		ImGui::DragInt("Num Meshes X", &s_NumMeshesX, 0.1f, 0, 256);
		ImGui::DragInt("Num Meshes Z", &s_NumMeshesZ, 0.1f, 0, 256);
		ImGui::DragFloat("Spacing", &s_MeshSpacing, 0.1f, 0.0f, 1024.0f);

		ImGui::Checkbox("Use Normal Maps", &m_UseNormalMaps);
		ImGui::DragFloat("Ambient Multiplier", &m_AmbientMultiplier, 0.001f, 0.0f, 1.0f);
		ImGui::DragFloat3("Light Position", m_LightPosition.GetPointer());
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