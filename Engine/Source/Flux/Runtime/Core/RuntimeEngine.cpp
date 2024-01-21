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

		static const char* s_VertexShaderSource =
			"#version 450 core\n"
			"layout(location = 0) in vec3 a_Position;\n"
			"layout(location = 1) in vec3 a_Color;\n"
			"layout(location = 0) out vec3 v_Color;\n"
			"uniform mat4 u_Transform;\n"
			"void main()\n"
			"{\n"
			"    v_Color = a_Color;\n"
			"    gl_Position = u_Transform * vec4(a_Position, 1.0);\n"
			"}\n";

		static const char* s_FragmentShaderSource =
			"#version 450 core\n"
			"layout(location = 0) out vec4 o_Color;\n"
			"layout(location = 0) in vec3 v_Color;\n"
			"void main()\n"
			"{\n"
			"    o_Color = vec4(v_Color, 1.0);\n"
			"}\n";

		m_Shader = Shader::Create(s_VertexShaderSource, s_FragmentShaderSource);

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
		m_IndexBuffer->Bind();

		Matrix transform(1.0f);

		static float zRotation = 0.0f;
		zRotation += 10.0f * deltaTime;

		transform.SetEulerAngles(Vector3(0.0f, 0.0f, zRotation) * Math::DegToRad);

		m_Shader->Bind();
		m_Shader->SetUniform("u_Transform", transform);

		m_Pipeline->DrawIndexed(
			m_IndexBuffer->GetDataType(),
			m_IndexBuffer->GetSize() / Utils::IndexBufferDataTypeSize(IndexBufferDataType::UInt32)
		);
	}

}