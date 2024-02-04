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
			{ "a_Normal", VertexElementFormat::Float3 }
		};
		pipelineCreateInfo.DepthTest = true;
		pipelineCreateInfo.DepthWrite = true;
		pipelineCreateInfo.BackfaceCulling = true;
		m_Pipeline = GraphicsPipeline::Create(pipelineCreateInfo);

		m_Mesh = Mesh::LoadFromFile("Resources/Meshes/test.fbx");
	}

	void RuntimeEngine::OnShutdown()
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		m_Shader = nullptr;
		m_Pipeline = nullptr;
		m_Mesh = nullptr;
	}

	void RuntimeEngine::OnUpdate()
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		// Clear color (TODO: remove)
		FLUX_SUBMIT_RENDER_COMMAND([windowWidth = m_MainWindow->GetWidth(), windowHeight = m_MainWindow->GetHeight()]() mutable
		{
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

			glDisable(GL_SCISSOR_TEST);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glEnable(GL_SCISSOR_TEST);

			glViewport(0, 0, windowWidth, windowHeight);
		});

		// TODO
		float deltaTime = m_DeltaTime;
		m_EditorCamera.OnUpdate(deltaTime);

		m_Mesh->GetVertexBuffer()->Bind();
		m_Pipeline->Bind();
		m_Pipeline->Scissor(0, 0, m_MainWindow->GetWidth(), m_MainWindow->GetHeight());
		m_Mesh->GetIndexBuffer()->Bind();

		m_Shader->Bind();
		m_Shader->SetUniform("u_ViewProjectionMatrix", m_EditorCamera.GetProjectionMatrix() * m_EditorCamera.GetViewMatrix());

		auto& properties = m_Mesh->GetProperties();

		Matrix4x4 meshTransform = Math::BuildTransformationMatrix({ 0.0f, 0.0f, 2.0f }, Vector3(0.0f), Vector3(1.0f));

		for (uint32 i = 0; i < (uint32)properties.Submeshes.size(); i++)
		{
			auto& submesh = properties.Submeshes[i];

			m_Shader->SetUniform("u_Transform", meshTransform * submesh.WorldTransform);
			m_Pipeline->DrawIndexed(
				submesh.IndexFormat,
				submesh.IndexCount, 
				submesh.StartIndexLocation,
				submesh.BaseVertexLocation
			);
		}
	}

	void RuntimeEngine::OnImGuiRender()
	{
		ImGui::Begin("Debug");
		ImGui::Text("Camera Position: [%.2f, %.2f, %.2f]", m_EditorCamera.GetPosition().X, m_EditorCamera.GetPosition().Y, m_EditorCamera.GetPosition().Z);
		ImGui::Text("Camera Rotation: [%.2f, %.2f, %.2f]", m_EditorCamera.GetRotation().X, m_EditorCamera.GetRotation().Y, m_EditorCamera.GetRotation().Z);
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