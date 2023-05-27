#include "FluxPCH.h"
#include "UIRenderer.h"

#include "Flux/Runtime/Renderer/Renderer.h"

namespace Flux {

	UIRenderer::UIRenderer()
	{
		uint32 framesInFlight = Renderer::GetFramesInFlight();

		CommandBufferCreateInfo commandBufferCreateInfo;
		commandBufferCreateInfo.CreateFromSwapchain = true;
		m_CommandBuffer = CommandBuffer::Create(commandBufferCreateInfo);

		FramebufferCreateInfo framebufferCreateInfo;
		framebufferCreateInfo.ClearColor = { 0.1f, 0.1f, 0.1f, 1.0f };
		framebufferCreateInfo.SwapchainTarget = true;
		m_SwapchainFramebuffer = Framebuffer::Create(framebufferCreateInfo);

		m_Shader = Shader::Create("Resources/Shaders/Shader.glsl");

		GraphicsPipelineCreateInfo pipelineCreateInfo;
		pipelineCreateInfo.Shader = m_Shader;
		pipelineCreateInfo.Framebuffer = m_SwapchainFramebuffer;
		m_Pipeline = GraphicsPipeline::Create(pipelineCreateInfo);

		m_VertexBuffer.resize(framesInFlight);
		m_VertexStorage.resize(framesInFlight);
		for (uint32 i = 0; i < framesInFlight; i++)
		{
			m_VertexBuffer[i] = VertexBuffer::Create(s_MaxVertices * sizeof(Vertex));

			m_VertexStorage[i] = new Vertex[s_MaxVertices];
			memset(m_VertexStorage[i], 0, sizeof(Vertex) * s_MaxVertices);
		}

		uint32* indices = new uint32[s_MaxIndices];

		uint32 offset = 0;
		for (uint32 i = 0; i < s_MaxIndices; i += 6)
		{
			indices[i + 0] = offset + 0;
			indices[i + 1] = offset + 1;
			indices[i + 2] = offset + 2;

			indices[i + 3] = offset + 2;
			indices[i + 4] = offset + 3;
			indices[i + 5] = offset + 0;

			offset += 4;
		}

		m_IndexBuffer = IndexBuffer::Create(indices, s_MaxIndices * sizeof(uint32));

		delete[] indices;
	}

	UIRenderer::~UIRenderer()
	{
		uint32 framesInFlight = Renderer::GetFramesInFlight();

		for (uint32 i = 0; i < framesInFlight; i++)
			delete m_VertexStorage[i];
	}

	void UIRenderer::BeginRendering()
	{
		uint32 frameIndex = Renderer::GetCurrentFrameIndex();

		m_VertexPointer = m_VertexStorage[frameIndex];
		m_IndexCount = 0;
	
		// Testing code
		{
			const float xPadding = 8.0f;
			const float yPadding = 6.0f;
			const float spacing = 6.0f;

			const float windowWidth = 1280.0f;
			const float windowHeight = 720.0f;

			float xs = 57.0f;
			float ys = 18.0f;
			float menuBarHeight = 20.0f;
			DrawRect(
				xPadding,
				windowHeight - menuBarHeight - ys - yPadding,
				xs + xPadding,
				windowHeight - menuBarHeight - yPadding,
				{ 0.22f, 0.22f, 0.22f, 1.0f }
			);

			float xs2 = 32.0f;
			DrawRect(
				xPadding + xs + spacing,
				windowHeight - menuBarHeight - ys - yPadding,
				xPadding + xs + spacing + xs2,
				windowHeight - menuBarHeight - yPadding,
				{ 0.22f, 0.22f, 0.22f, 1.0f }
			);

			DrawRect(50.0f, 50.0f, 200.0f, 100.0f, { 0.82f, 0.22f, 0.22f, 1.0f });
			DrawRect(300.0f, 300.0f, 400.0f, 400.0f, { 0.82f, 0.82f, 0.22f, 1.0f });
		}
	}

	void UIRenderer::EndRendering()
	{
		m_CommandBuffer->Begin();
		Renderer::BeginRenderPass(m_CommandBuffer, m_SwapchainFramebuffer);

		uint32 frameIndex = Renderer::GetCurrentFrameIndex();

		if (m_IndexCount > 0)
		{
			uint32 dataSize = (uint8*)m_VertexPointer - (uint8*)m_VertexStorage[frameIndex];
			m_VertexBuffer[frameIndex]->SetData(m_CommandBuffer, m_VertexStorage[frameIndex], dataSize);

			m_VertexBuffer[frameIndex]->Bind(m_CommandBuffer);
			m_Pipeline->Bind(m_CommandBuffer);
			m_IndexBuffer->Bind(m_CommandBuffer);

			glm::mat4 projectionMatrix = glm::ortho(0.0f, (float)m_SwapchainFramebuffer->GetWidth(), 0.0f, (float)m_SwapchainFramebuffer->GetHeight());
			FLUX_SUBMIT_RENDER_COMMAND([commandBuffer = m_CommandBuffer, pipeline = m_Pipeline, projectionMatrix]()
			{
				pipeline->RT_SetPushConstant(commandBuffer, ShaderStage::Vertex, &(projectionMatrix[0].x), 64);
			});

			m_Pipeline->DrawIndexed(m_CommandBuffer, m_IndexCount);
		}

		Renderer::EndRenderPass(m_CommandBuffer);
		m_CommandBuffer->End();
	}

	void UIRenderer::DrawRect(float minX, float minY, float maxX, float maxY, const glm::vec4& color)
	{
		m_VertexPointer->Position = { minX, minY, 0.0f };
		m_VertexPointer->Color = color;
		m_VertexPointer++;

		m_VertexPointer->Position = { maxX, minY, 0.0f };
		m_VertexPointer->Color = color;
		m_VertexPointer++;

		m_VertexPointer->Position = { maxX, maxY, 0.0f };
		m_VertexPointer->Color = color;
		m_VertexPointer++;

		m_VertexPointer->Position = { minX, maxY, 0.0f };
		m_VertexPointer->Color = color;
		m_VertexPointer++;

		m_IndexCount += 6;
	}

}