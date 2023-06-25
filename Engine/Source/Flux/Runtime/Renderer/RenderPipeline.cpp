#include "FluxPCH.h"
#include "RenderPipeline.h"

#include "Flux/Runtime/Core/Engine.h"

#include "Renderer.h"

namespace Flux {

	static const bool s_SwapchainTarget = true;

	ForwardRenderPipeline::ForwardRenderPipeline()
	{
		FLUX_CHECK_IS_MAIN_THREAD();

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
		Ref<Framebuffer> framebuffer = Framebuffer::Create(framebufferCreateInfo);

		// Quads
		{
			m_QuadShader = Shader::Create("Resources/Shaders/Shader.glsl");

			GraphicsPipelineCreateInfo pipelineCreateInfo;
			pipelineCreateInfo.Shader = m_QuadShader;
			pipelineCreateInfo.Framebuffer = framebuffer;
			pipelineCreateInfo.DebugLabel = "Quad Pipeline";
			m_QuadPipeline = GraphicsPipeline::Create(pipelineCreateInfo);

			for (uint32 frameIndex = 0; frameIndex < Renderer::GetFramesInFlight(); frameIndex++)
				m_QuadPipeline->SetUniformBuffer("Camera", Renderer::GetUniformBuffer("Camera", frameIndex), frameIndex);

			m_QuadPipeline->Bake();

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

			m_QuadTextureSlots[0] = Renderer::GetWhiteTexture();
		}
	}

	ForwardRenderPipeline::~ForwardRenderPipeline()
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		uint32 framesInFlight = Renderer::GetFramesInFlight();

		for (uint32 i = 0; i < framesInFlight; i++)
			delete m_QuadVertexStorage[i];
	}

	void ForwardRenderPipeline::BeginRendering()
	{
		FLUX_CHECK_IS_MAIN_THREAD();
	}

	void ForwardRenderPipeline::EndRendering()
	{
		FLUX_CHECK_IS_MAIN_THREAD();
	}

	void ForwardRenderPipeline::BeginRendering2D()
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		uint32 frameIndex = Renderer::GetCurrentFrameIndex();

		m_QuadVertexPointer = m_QuadVertexStorage[frameIndex];
		m_QuadIndexCount = 0;

		m_QuadTextureSlotIndex = 1;
		for (uint32_t i = m_QuadTextureSlotIndex; i < m_QuadTextureSlots.size(); i++)
			m_QuadTextureSlots[i] = nullptr;

		Ref<UniformBuffer> cameraUniformBuffer = Renderer::GetUniformBuffer("Camera");
		if (cameraUniformBuffer)
		{
			glm::mat4 viewMatrix = m_CameraSettings.ViewMatrix;
			glm::mat4 projectionMatrix = m_CameraSettings.ProjectionMatrix;
			glm::mat4 viewProjectionMatrix = projectionMatrix * viewMatrix;
			glm::mat4 inverseViewProjectionMatrix = glm::inverse(viewProjectionMatrix);

			cameraUniformBuffer->Set("u_ViewProjectionMatrix", viewProjectionMatrix);
			cameraUniformBuffer->Set("u_InverseViewProjectionMatrix", inverseViewProjectionMatrix);
			cameraUniformBuffer->Set("u_ProjectionMatrix", projectionMatrix);
			cameraUniformBuffer->Set("u_ViewMatrix", viewMatrix);
		}
	}

	void ForwardRenderPipeline::EndRendering2D()
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		m_CommandBuffer->Begin();

		uint32 frameIndex = Renderer::GetCurrentFrameIndex();

		if (m_ViewportWidth > 0 && m_ViewportHeight > 0)
		{
			if (m_QuadIndexCount > 0)
			{
				uint32 dataSize = static_cast<uint32>((uint8*)m_QuadVertexPointer - (uint8*)m_QuadVertexStorage[frameIndex]);
				m_QuadVertexBuffer[frameIndex]->SetData(m_CommandBuffer, m_QuadVertexStorage[frameIndex], dataSize);

				Renderer::BeginRenderPass(m_CommandBuffer, m_QuadPipeline);
				Renderer::RenderGeometry(m_CommandBuffer, m_QuadVertexBuffer[frameIndex], m_QuadIndexBuffer, m_QuadIndexCount);
				Renderer::EndRenderPass(m_CommandBuffer);
			}
		}

		m_CommandBuffer->End();
		m_CommandBuffer->Submit();
	}

	void ForwardRenderPipeline::DrawQuad(const glm::mat4& transform, const glm::vec4& color)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		m_QuadVertexPointer->Position = transform * glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f);
		m_QuadVertexPointer->Color = color;
		m_QuadVertexPointer->TexCoord = { 0.0f, 1.0f };
		m_QuadVertexPointer->TextureIndex = 0.0f;
		m_QuadVertexPointer++;

		m_QuadVertexPointer->Position = transform * glm::vec4(0.5f, -0.5f, 0.0f, 1.0f);
		m_QuadVertexPointer->Color = color;
		m_QuadVertexPointer->TexCoord = { 1.0f, 1.0f };
		m_QuadVertexPointer->TextureIndex = 0.0f;
		m_QuadVertexPointer++;

		m_QuadVertexPointer->Position = transform * glm::vec4(0.5f, 0.5f, 0.0f, 1.0f);
		m_QuadVertexPointer->Color = color;
		m_QuadVertexPointer->TexCoord = { 1.0f, 0.0f };
		m_QuadVertexPointer->TextureIndex = 0.0f;
		m_QuadVertexPointer++;

		m_QuadVertexPointer->Position = transform * glm::vec4(-0.5f, 0.5f, 0.0f, 1.0f);
		m_QuadVertexPointer->Color = color;
		m_QuadVertexPointer->TexCoord = { 0.0f, 0.0f };
		m_QuadVertexPointer->TextureIndex = 0.0f;
		m_QuadVertexPointer++;

		m_QuadIndexCount += 6;
	}

	void ForwardRenderPipeline::DrawQuad(const glm::vec3& position, float rotation, const glm::vec2& scale, const glm::vec4& color)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { scale, 1.0f });

		DrawQuad(transform, color);
	}

	void ForwardRenderPipeline::DrawQuad(const glm::vec3& position, const glm::vec2& scale, const glm::vec4& color)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { scale, 1.0f });

		DrawQuad(transform, color);
	}

	void ForwardRenderPipeline::DrawQuad(const glm::mat4& transform, Ref<Texture2D> texture, const glm::vec4& color)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		float textureIndex = 0.0f;
		for (uint32 i = 1; i < m_QuadTextureSlotIndex; i++)
		{
			if (m_QuadTextureSlots[i].Equals(texture))
			{
				textureIndex = (float)i;
				break;
			}
		}

		if (textureIndex == 0.0f)
		{
			textureIndex = (float)m_QuadTextureSlotIndex;
			m_QuadTextureSlots[m_QuadTextureSlotIndex] = texture;
			m_QuadTextureSlotIndex++;
		}

		m_QuadVertexPointer->Position = transform * glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f);
		m_QuadVertexPointer->Color = color;
		m_QuadVertexPointer->TexCoord = { 0.0f, 1.0f };
		m_QuadVertexPointer->TextureIndex = textureIndex;
		m_QuadVertexPointer++;

		m_QuadVertexPointer->Position = transform * glm::vec4(0.5f, -0.5f, 0.0f, 1.0f);
		m_QuadVertexPointer->Color = color;
		m_QuadVertexPointer->TexCoord = { 1.0f, 1.0f };
		m_QuadVertexPointer->TextureIndex = textureIndex;
		m_QuadVertexPointer++;

		m_QuadVertexPointer->Position = transform * glm::vec4(0.5f, 0.5f, 0.0f, 1.0f);
		m_QuadVertexPointer->Color = color;
		m_QuadVertexPointer->TexCoord = { 1.0f, 0.0f };
		m_QuadVertexPointer->TextureIndex = textureIndex;
		m_QuadVertexPointer++;

		m_QuadVertexPointer->Position = transform * glm::vec4(-0.5f, 0.5f, 0.0f, 1.0f);
		m_QuadVertexPointer->Color = color;
		m_QuadVertexPointer->TexCoord = { 0.0f, 0.0f };
		m_QuadVertexPointer->TextureIndex = textureIndex;
		m_QuadVertexPointer++;

		m_QuadIndexCount += 6;
	}

	void ForwardRenderPipeline::DrawQuad(const glm::vec3& position, float rotation, const glm::vec2& scale, Ref<Texture2D> texture, const glm::vec4& color)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::rotate(glm::mat4(1.0f), glm::radians(rotation), { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { scale, 1.0f });

		DrawQuad(transform, texture, color);
	}

	void ForwardRenderPipeline::DrawQuad(const glm::vec3& position, const glm::vec2& scale, Ref<Texture2D> texture, const glm::vec4& color)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
			* glm::scale(glm::mat4(1.0f), { scale, 1.0f });

		DrawQuad(transform, texture, color);
	}

	void ForwardRenderPipeline::SetViewportSize(uint32 width, uint32 height)
	{
		FLUX_CHECK_IS_MAIN_THREAD();
		FLUX_ASSERT(width > 0 && height > 0);

		if (m_ViewportWidth != width || m_ViewportHeight != height)
		{
			m_QuadPipeline->GetFramebuffer()->Resize(width, height);

			m_ViewportWidth = width;
			m_ViewportHeight = height;
		}
	}

}