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
			-0.5f,  0.5f,  0.5f, -1.0f, 0.0f,  0.0f,
			-0.5f,  0.5f, -0.5f, -1.0f, 0.0f,  0.0f,
			-0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,
			-0.5f, -0.5f,  0.5f, -1.0f, 0.0f,  0.0f,

			 0.5f, -0.5f,  0.5f, 1.0f,  0.0f,  0.0f,
			 0.5f, -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,
			 0.5f,  0.5f, -0.5f, 1.0f,  0.0f,  0.0f,
			 0.5f,  0.5f,  0.5f, 1.0f,  0.0f,  0.0f,

			-0.5f, -0.5f,  0.5f, 0.0f, -1.0f,  0.0f,
			-0.5f, -0.5f, -0.5f, 0.0f, -1.0f,  0.0f,
			 0.5f, -0.5f, -0.5f, 0.0f, -1.0f,  0.0f,
			 0.5f, -0.5f,  0.5f, 0.0f, -1.0f,  0.0f,

			 0.5f, 0.5f,   0.5f, 0.0f,  1.0f,  0.0f,
			 0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,
			-0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,
			-0.5f, 0.5f,   0.5f, 0.0f,  1.0f,  0.0f,
									   
			-0.5f,  0.5f, -0.5f, 0.0f,  0.0f, -1.0f,
			 0.5f,  0.5f, -0.5f, 0.0f,  0.0f, -1.0f,
			 0.5f, -0.5f, -0.5f, 0.0f,  0.0f, -1.0f,
			-0.5f, -0.5f, -0.5f, 0.0f,  0.0f, -1.0f,

			-0.5f,  0.5f,  0.5f, 0.0f,  0.0f,  1.0f,
			-0.5f, -0.5f,  0.5f, 0.0f,  0.0f,  1.0f,
			 0.5f, -0.5f,  0.5f, 0.0f,  0.0f,  1.0f,
			 0.5f,  0.5f,  0.5f, 0.0f,  0.0f,  1.0f
		};

		uint32* indices = new uint32[6 * 6];

		uint32 offset = 0;
		for (uint32 i = 0; i < 6 * 6; i += 6)
		{
			indices[i + 0] = offset + 0;
			indices[i + 1] = offset + 1;
			indices[i + 2] = offset + 2;

			indices[i + 3] = offset + 2;
			indices[i + 4] = offset + 3;
			indices[i + 5] = offset + 0;

			offset += 4;
		}

		m_VertexBuffer = VertexBuffer::Create(vertices, sizeof(vertices));
		m_IndexBuffer = IndexBuffer::Create(indices, 6 * 6 * sizeof(uint32), IndexBufferDataType::UInt32);

		delete[] indices;

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
	}

	void RuntimeEngine::OnShutdown()
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		m_Shader = nullptr;
		m_Pipeline = nullptr;
		m_VertexBuffer = nullptr;
		m_IndexBuffer = nullptr;
	}

	static float s_QuadRotationX = 0.0f;
	static float s_QuadRotationZ = 0.0f;
	static Vector3 s_CameraPosition(0.0f, 0.0f, -2.5f);
	static float s_CameraFov = 60.0f;

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
		Matrix4x4 projectionMatrix = Matrix4x4::Perspective(s_CameraFov, aspectRatio, 0.1f, 1000.0f);

		if (Input::GetKey(KeyCode::Up))
			s_CameraPosition.Y += deltaTime;
		if (Input::GetKey(KeyCode::Down))
			s_CameraPosition.Y -= deltaTime;
		if (Input::GetKey(KeyCode::Left))
			s_CameraPosition.X -= deltaTime;
		if (Input::GetKey(KeyCode::Right))
			s_CameraPosition.X += deltaTime;

		Matrix4x4 viewMatrix = Matrix4x4::Translate(-s_CameraPosition);
		Matrix4x4 viewProjectionMatrix = projectionMatrix * viewMatrix;

		s_QuadRotationX += 10.0f * deltaTime;
		s_QuadRotationZ += 5.0f * deltaTime;

		Matrix4x4 transform = Math::BuildTransformationMatrix({}, Vector3(
			s_QuadRotationX * Math::DegToRad, 
			0.0f, 
			s_QuadRotationZ * Math::DegToRad)
		);

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

		ImGui::Text("X rotation: %f deg, %f rad", fmod(s_QuadRotationX, 360.0f), s_QuadRotationX * Math::DegToRad);
		ImGui::Text("Z rotation: %f deg, %f rad", fmod(s_QuadRotationZ, 360.0f), s_QuadRotationZ * Math::DegToRad);

		ImGui::DragFloat("##CameraPositionX", &s_CameraPosition.X, 0.01f);
		ImGui::DragFloat("##CameraPositionY", &s_CameraPosition.Y, 0.01f);
		ImGui::DragFloat("##CameraPositionZ", &s_CameraPosition.Z, 0.01f);
		ImGui::DragFloat("Field of view", &s_CameraFov, 0.1f);

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