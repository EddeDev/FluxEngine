#include "FluxPCH.h"
#include "RenderPipeline.h"

#include "Flux/Runtime/Core/Engine.h"

#include "Renderer.h"

namespace Flux {

	static const bool s_SwapchainTarget = true;

	ForwardRenderPipeline::ForwardRenderPipeline()
	{
		m_ViewportWidth = Engine::Get().GetSwapchain()->GetWidth();
		m_ViewportHeight = Engine::Get().GetSwapchain()->GetHeight();

		uint32 framesInFlight = Renderer::GetFramesInFlight();

		CommandBufferCreateInfo commandBufferCreateInfo;
		commandBufferCreateInfo.Count = framesInFlight;
		commandBufferCreateInfo.Transient = true;
		commandBufferCreateInfo.CreateFromSwapchain = s_SwapchainTarget; // Optional?
		commandBufferCreateInfo.DebugLabel = "Forward RP-CommandBuffer";
		m_CommandBuffer = CommandBuffer::Create(commandBufferCreateInfo);

		FramebufferCreateInfo framebufferCreateInfo;
		framebufferCreateInfo.Attachments = {
			PixelFormat::RGBA,
			PixelFormat::Depth24Stencil8
		};
		framebufferCreateInfo.SwapchainTarget = s_SwapchainTarget;
		framebufferCreateInfo.DebugLabel = "Main FB";
		m_Framebuffer = Framebuffer::Create(framebufferCreateInfo);

		// Quads
		{
			m_QuadShader = Shader::Create("Resources/Shaders/Shader.glsl");

			GraphicsPipelineCreateInfo pipelineCreateInfo;
			pipelineCreateInfo.Shader = m_QuadShader;
			pipelineCreateInfo.Framebuffer = m_Framebuffer;
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
	}

	ForwardRenderPipeline::~ForwardRenderPipeline()
	{
		uint32 framesInFlight = Renderer::GetFramesInFlight();

		for (uint32 i = 0; i < framesInFlight; i++)
			delete m_QuadVertexStorage[i];
	}

	void ForwardRenderPipeline::BeginRendering()
	{
	}

	void ForwardRenderPipeline::EndRendering()
	{
	}

	void ForwardRenderPipeline::BeginRendering2D()
	{
		uint32 frameIndex = Renderer::GetCurrentFrameIndex();

		m_QuadVertexPointer = m_QuadVertexStorage[frameIndex];
		m_QuadIndexCount = 0;
	}

	void ForwardRenderPipeline::EndRendering2D()
	{
		m_CommandBuffer->Begin();

		uint32 frameIndex = Renderer::GetCurrentFrameIndex();

		Renderer::BeginRenderPass(m_CommandBuffer, m_Framebuffer);

		if (m_QuadIndexCount > 0)
		{
			uint32 dataSize = static_cast<uint32>((uint8*)m_QuadVertexPointer - (uint8*)m_QuadVertexStorage[frameIndex]);
			m_QuadVertexBuffer[frameIndex]->SetData(m_CommandBuffer, m_QuadVertexStorage[frameIndex], dataSize);

			m_QuadVertexBuffer[frameIndex]->Bind(m_CommandBuffer);
			m_QuadPipeline->Bind(m_CommandBuffer);
			m_QuadIndexBuffer->Bind(m_CommandBuffer);

			float aspectRatio = (float)m_Framebuffer->GetWidth() / (float)m_Framebuffer->GetHeight();

			// glm::mat4 projectionMatrix = glm::ortho(0.0f, (float)m_Framebuffer->GetWidth(), 0.0f, (float)m_Framebuffer->GetHeight());
			glm::mat4 projectionMatrix = glm::perspective(glm::radians(70.0f), aspectRatio, 0.01f, 1000.0f);
			m_QuadPipeline->SetPushConstant(m_CommandBuffer, ShaderStage::Vertex, &(projectionMatrix[0].x), 64);

			m_QuadPipeline->DrawIndexed(m_CommandBuffer, m_QuadIndexCount);
		}

		Renderer::EndRenderPass(m_CommandBuffer);

		m_CommandBuffer->End();
		m_CommandBuffer->Submit();
	}

	void ForwardRenderPipeline::DrawQuad(const glm::mat4& transform, const glm::vec4& color)
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

	void ForwardRenderPipeline::DrawQuad(const glm::vec3& position, float rotation, const glm::vec2& scale, const glm::vec4& color)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { scale, 1.0f });

		DrawQuad(transform, color);
	}

	void ForwardRenderPipeline::DrawQuad(const glm::vec3& position, const glm::vec2& scale, const glm::vec4& color)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { scale, 1.0f });

		DrawQuad(transform, color);
	}

	void ForwardRenderPipeline::SetViewportSize(uint32 width, uint32 height)
	{
		FLUX_ASSERT(width > 0 && height > 0);

		if (m_ViewportWidth != width || m_ViewportHeight != height)
		{
			m_Framebuffer->Resize(width, height);

			m_ViewportWidth = width;
			m_ViewportHeight = height;
		}
	}

}