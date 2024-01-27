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

		float vertices[] =
		{
			-0.5f,  0.5f, 0.0f,
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.5f,  0.5f, 0.0f
		};

		uint32 indices[] = {
			0, 1, 3,
			3, 1, 2
		};

		m_VertexBuffer = VertexBuffer::Create(vertices, sizeof(vertices));
		m_IndexBuffer = IndexBuffer::Create(indices, sizeof(indices), IndexBufferDataType::UInt32);

		m_Shader = Shader::Create("Resources/Shaders/Shader.glsl");

		GraphicsPipelineCreateInfo pipelineCreateInfo;
		pipelineCreateInfo.VertexDeclaration = {
			{ "a_Position", VertexElementFormat::Float3 }
		};

		m_Pipeline = GraphicsPipeline::Create(pipelineCreateInfo);
	}

	void RuntimeEngine::OnShutdown()
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		m_Shader = nullptr;
		m_Pipeline = nullptr;
		m_VertexBuffer = nullptr;
		m_IndexBuffer = nullptr;
	}

	static float s_QuadRotation = 0.0f;
	static Vector3 cameraPosition(0.0f, 0.0f, -2.5f);
	static float cameraFov = 60.0f;

	void RuntimeEngine::OnUpdate(float deltaTime)
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

		m_VertexBuffer->Bind();
		m_Pipeline->Bind();
		m_Pipeline->Scissor(0, 0, m_MainWindow->GetWidth(), m_MainWindow->GetHeight());
		m_IndexBuffer->Bind();

		float aspectRatio = (float)m_MainWindow->GetWidth() / (float)m_MainWindow->GetHeight();
		// Matrix4x4 projectionMatrix = Matrix4x4::Ortho(-aspectRatio * s_CameraZoomLevel, aspectRatio * s_CameraZoomLevel, -s_CameraZoomLevel, s_CameraZoomLevel);
		Matrix4x4 projectionMatrix = Matrix4x4::Perspective(cameraFov, aspectRatio, 0.1f, 1000.0f);
		
		if (Input::GetKey(KeyCode::Up))
			cameraPosition.Y += deltaTime;
		if (Input::GetKey(KeyCode::Down))
			cameraPosition.Y -= deltaTime;
		if (Input::GetKey(KeyCode::Left))
			cameraPosition.X -= deltaTime;
		if (Input::GetKey(KeyCode::Right))
			cameraPosition.X += deltaTime;

		Matrix4x4 viewMatrix = Matrix4x4(1.0f).Translate(-cameraPosition);

		Matrix4x4 viewProjectionMatrix = projectionMatrix * viewMatrix;

		s_QuadRotation += 10.0f * deltaTime;

		Matrix4x4 transform = Math::BuildTransformationMatrix({}, { 0.0f, 0.0f, s_QuadRotation * Math::DegToRad });

		m_Shader->Bind();

		m_Shader->SetUniform("u_Transform", viewProjectionMatrix * transform);
		m_Pipeline->DrawIndexed(
			m_IndexBuffer->GetDataType(),
			m_IndexBuffer->GetSize() / Utils::IndexBufferDataTypeSize(IndexBufferDataType::UInt32)
		);
	}

	void RuntimeEngine::OnImGuiRender()
	{
		ImGui::Begin("Debug");

		ImGui::Text("Z rotation: %f deg, %f rad", fmod(s_QuadRotation, 360.0f), s_QuadRotation * Math::DegToRad);

		ImGui::DragFloat("##CameraPositionX", &cameraPosition.X, 0.01f);
		ImGui::DragFloat("##CameraPositionY", &cameraPosition.Y, 0.01f);
		ImGui::DragFloat("##CameraPositionZ", &cameraPosition.Z, 0.01f);
		ImGui::DragFloat("Field of view", &cameraFov, 0.1f);

		ImGui::End();
	}

	void RuntimeEngine::OnEvent(Event& event)
	{
		EventHandler handler(event);
		handler.Bind<WindowCloseEvent>(FLUX_BIND_CALLBACK(OnWindowCloseEvent, this));
	}

	void RuntimeEngine::OnWindowCloseEvent(WindowCloseEvent& event)
	{
		if (event.GetWindow() == m_MainWindow)
			Close();
	}

}