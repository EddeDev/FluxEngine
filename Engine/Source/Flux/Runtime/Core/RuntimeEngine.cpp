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
			-0.5f,  0.5f, 0.0f, 0.8, 0.2, 0.2,
			-0.5f, -0.5f, 0.0f, 0.8, 0.8, 0.2,
			 0.5f, -0.5f, 0.0f, 0.2, 0.8, 0.8,
			 0.5f,  0.5f, 0.0f, 0.8, 0.2, 0.8
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
			{ "a_Position", VertexElementFormat::Float3 },
			{ "a_Color", VertexElementFormat::Float3 }
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

	static float zRotation = 0.0f;

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

		zRotation += 10.0f * deltaTime;

		Matrix4x4 transform = Math::BuildTransformationMatrix(
			{ 0.5f, 0.0f, 0.0f },
			Vector3(0.0f, 0.0f, zRotation) * Math::DegToRad,
			{ 0.3f, 0.3f, 0.3f }
		);

		m_Shader->Bind();

		m_Shader->SetUniform("u_Transform", transform);
		m_Pipeline->DrawIndexed(
			m_IndexBuffer->GetDataType(),
			m_IndexBuffer->GetSize() / Utils::IndexBufferDataTypeSize(IndexBufferDataType::UInt32)
		);

		transform = Math::BuildTransformationMatrix(
			{ -0.5f, 0.0f, 0.0f },
			Vector3(0.0f, 0.0f, zRotation) * Math::DegToRad,
			{ 0.3f, 0.3f, 0.3f }
		);

		m_Shader->SetUniform("u_Transform", transform);
		m_Pipeline->DrawIndexed(
			m_IndexBuffer->GetDataType(),
			m_IndexBuffer->GetSize() / Utils::IndexBufferDataTypeSize(IndexBufferDataType::UInt32)
		);
	}

	void RuntimeEngine::OnImGuiRender()
	{
		ImGui::Begin("Debug");

		float z = zRotation;
		ImGui::Text("Z rotation: %f deg, %f rad", z, z * Math::DegToRad);

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