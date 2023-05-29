#include "FluxPCH.h"
#include "BatchRenderer.h"

#include "Flux/Runtime/Renderer/Renderer.h"

#include <glm/gtx/quaternion.hpp>

namespace Flux {

	BatchRenderer::BatchRenderer()
	{
		uint32 framesInFlight = Renderer::GetFramesInFlight();

		CommandBufferCreateInfo commandBufferCreateInfo;
		commandBufferCreateInfo.Count = framesInFlight;
		commandBufferCreateInfo.CreateFromSwapchain = true;
		m_CommandBuffer = CommandBuffer::Create(commandBufferCreateInfo);

		FramebufferCreateInfo framebufferCreateInfo;
		framebufferCreateInfo.ClearColor = { 0.1f, 0.1f, 0.1f, 1.0f };
		framebufferCreateInfo.SwapchainTarget = true;
		m_SwapchainFramebuffer = Framebuffer::Create(framebufferCreateInfo);

		m_QuadShader = Shader::Create("Resources/Shaders/Shader.glsl");

		GraphicsPipelineCreateInfo pipelineCreateInfo;
		pipelineCreateInfo.Shader = m_QuadShader;
		pipelineCreateInfo.Framebuffer = m_SwapchainFramebuffer;
		m_QuadPipeline = GraphicsPipeline::Create(pipelineCreateInfo);

		m_QuadVertexBuffer.resize(framesInFlight);
		m_QuadVertexStorage.resize(framesInFlight);
		for (uint32 i = 0; i < framesInFlight; i++)
		{
			m_QuadVertexBuffer[i] = VertexBuffer::Create(s_MaxQuadVertices * sizeof(QuadVertex));

			m_QuadVertexStorage[i] = new QuadVertex[s_MaxQuadVertices];
			memset(m_QuadVertexStorage[i], 0, sizeof(QuadVertex) * s_MaxQuadVertices);
		}

		uint32* indices = new uint32[s_MaxQuadIndices];

		uint32 offset = 0;
		for (uint32 i = 0; i < s_MaxQuadIndices; i += 6)
		{
			indices[i + 0] = offset + 0;
			indices[i + 1] = offset + 1;
			indices[i + 2] = offset + 2;

			indices[i + 3] = offset + 2;
			indices[i + 4] = offset + 3;
			indices[i + 5] = offset + 0;

			offset += 4;
		}

		m_QuadIndexBuffer = IndexBuffer::Create(indices, s_MaxQuadIndices * sizeof(uint32));

		delete[] indices;
	}

	BatchRenderer::~BatchRenderer()
	{
		uint32 framesInFlight = Renderer::GetFramesInFlight();

		for (uint32 i = 0; i < framesInFlight; i++)
			delete m_QuadVertexStorage[i];
	}

	void BatchRenderer::BeginRendering()
	{
		uint32 frameIndex = Renderer::GetCurrentFrameIndex();

		m_QuadVertexPointer = m_QuadVertexStorage[frameIndex];
		m_QuadIndexCount = 0;
	}

	void BatchRenderer::EndRendering()
	{
		m_CommandBuffer->Begin();
		Renderer::BeginRenderPass(m_CommandBuffer, m_SwapchainFramebuffer);

		uint32 frameIndex = Renderer::GetCurrentFrameIndex();

		if (m_QuadIndexCount > 0)
		{
			uint32 dataSize = static_cast<uint32>((uint8*)m_QuadVertexPointer - (uint8*)m_QuadVertexStorage[frameIndex]);
			m_QuadVertexBuffer[frameIndex]->SetData(m_CommandBuffer, m_QuadVertexStorage[frameIndex], dataSize);

			m_QuadVertexBuffer[frameIndex]->Bind(m_CommandBuffer);
			m_QuadPipeline->Bind(m_CommandBuffer);
			m_QuadIndexBuffer->Bind(m_CommandBuffer);

			glm::mat4 projectionMatrix = glm::ortho(0.0f, (float)m_SwapchainFramebuffer->GetWidth(), 0.0f, (float)m_SwapchainFramebuffer->GetHeight());
			FLUX_SUBMIT_RENDER_COMMAND([commandBuffer = m_CommandBuffer, pipeline = m_QuadPipeline, projectionMatrix]()
			{
				pipeline->RT_SetPushConstant(commandBuffer, ShaderStage::Vertex, &(projectionMatrix[0].x), 64);
			});

			m_QuadPipeline->DrawIndexed(m_CommandBuffer, m_QuadIndexCount);
		}

		Renderer::EndRenderPass(m_CommandBuffer);
		m_CommandBuffer->End();
	}

	void BatchRenderer::DrawQuad(const glm::mat4& transform, const glm::vec4& color)
	{
		m_QuadVertexPointer->Position = transform * glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f);
		m_QuadVertexPointer->Color = color;
		m_QuadVertexPointer++;

		m_QuadVertexPointer->Position = transform * glm::vec4(0.5f, -0.5f, 0.0f, 1.0f);
		m_QuadVertexPointer->Color = color;
		m_QuadVertexPointer++;

		m_QuadVertexPointer->Position = transform * glm::vec4(0.5f, 0.5f, 0.0f, 1.0f);
		m_QuadVertexPointer->Color = color;
		m_QuadVertexPointer++;

		m_QuadVertexPointer->Position = transform * glm::vec4(-0.5f, 0.5f, 0.0f, 1.0f);
		m_QuadVertexPointer->Color = color;
		m_QuadVertexPointer++;

		m_QuadIndexCount += 6;
	}

	void BatchRenderer::DrawQuad(const glm::vec3& position, float rotation, const glm::vec2& scale, const glm::vec4& color)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { scale, 1.0f });

		DrawQuad(transform, color);
	}

	void BatchRenderer::DrawQuad(const glm::vec3& position, const glm::vec2& scale, const glm::vec4& color)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { scale, 1.0f });

		DrawQuad(transform, color);
	}

	void BatchRenderer::DrawRect(float minX, float minY, float maxX, float maxY, const glm::vec4& color)
	{
		m_QuadVertexPointer->Position = { minX, minY, 0.0f };
		m_QuadVertexPointer->Color = color;
		m_QuadVertexPointer++;

		m_QuadVertexPointer->Position = { maxX, minY, 0.0f };
		m_QuadVertexPointer->Color = color;
		m_QuadVertexPointer++;

		m_QuadVertexPointer->Position = { maxX, maxY, 0.0f };
		m_QuadVertexPointer->Color = color;
		m_QuadVertexPointer++;

		m_QuadVertexPointer->Position = { minX, maxY, 0.0f };
		m_QuadVertexPointer->Color = color;
		m_QuadVertexPointer++;

		m_QuadIndexCount += 6;
	}

}