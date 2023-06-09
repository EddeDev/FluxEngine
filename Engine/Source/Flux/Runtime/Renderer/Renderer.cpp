#include "FluxPCH.h"
#include "Renderer.h"

#include "Flux/Runtime/Core/Engine.h"

#ifdef FLUX_HAS_VULKAN_SDK
	#include "Vulkan/VulkanResourceAllocator.h"
#endif

namespace Flux {

	struct RendererData
	{
		Ref<Texture2D> WhiteTexture;
		Ref<Texture2D> BlackTexture;

		Ref<VertexBuffer> FullscreenQuadVertexBuffer;
		Ref<IndexBuffer> FullscreenQuadIndexBuffer;

		Ref<GraphicsPipeline> ActivePipeline;

		uint32 CurrentFrameIndex = 0;
		uint32 CurrentQueueIndex = 0;

		uint32 FrameCount = 0;
	};

	static RendererData* s_Data = nullptr;

	static ResourceAllocator* CreateResourceAllocator()
	{
		switch (FLUX_CURRENT_GRAPHICS_API)
		{
#ifdef FLUX_HAS_VULKAN_SDK
		case GraphicsAPI::Vulkan: return new VulkanResourceAllocator();
#endif
		}
		FLUX_VERIFY(false, "Unknown Graphics API.");
		return nullptr;
	}

	void Renderer::Init()
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		s_Data = new RendererData();
		s_ResourceAllocator = CreateResourceAllocator();

		// White texture
		{
			TextureProperties textureProperties;
			textureProperties.GenerateMipmaps = false;
			textureProperties.DebugLabel = "White Texture";

			uint32 whiteTextureData = 0xffffffff;
			s_Data->WhiteTexture = Ref<Texture2D>::Create(1, 1, PixelFormat::RGBA, &whiteTextureData, textureProperties);
		}

		// Black texture
		{
			TextureProperties textureProperties;
			textureProperties.GenerateMipmaps = false;
			textureProperties.DebugLabel = "Black Texture";

			uint32 blackTextureData = 0xff000000;
			s_Data->BlackTexture = Ref<Texture2D>::Create(1, 1, PixelFormat::RGBA, &blackTextureData, textureProperties);
		}

		// Fullscreen quad
		{
			struct QuadVertex
			{
				glm::vec3 Position;
				glm::vec2 TexCoord;
			};

			struct QuadIndex
			{
				uint32 V1;
				uint32 V2;
				uint32 V3;
			};

			QuadVertex quadVertices[4];
			quadVertices[0] = { { -1.0f, -1.0f, 0.0f }, { 0.0f, 0.0f } };
			quadVertices[1] = { {  1.0f, -1.0f, 0.0f }, { 1.0f, 0.0f } };
			quadVertices[2] = { {  1.0f,  1.0f, 0.0f }, { 1.0f, 1.0f } };
			quadVertices[3] = { { -1.0f,  1.0f, 0.0f }, { 0.0f, 1.0f } };

			QuadIndex quadIndices[2];
			quadIndices[0] = { 0, 1, 2 };
			quadIndices[1] = { 2, 3, 0 };

			s_Data->FullscreenQuadVertexBuffer = VertexBuffer::Create(quadVertices, sizeof(QuadVertex) * 4);
			s_Data->FullscreenQuadIndexBuffer = IndexBuffer::Create(quadIndices, sizeof(QuadIndex) * 2);
		}
	}

	void Renderer::Shutdown()
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		FLUX_VERIFY(!s_Data);

		delete s_ResourceAllocator;
		s_ResourceAllocator = nullptr;
	}

	void Renderer::DestroyResources()
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		FLUX_VERIFY(s_ResourceAllocator);

		s_Shaders.clear();

		s_UniformBufferBindings.clear();
		s_UniformBuffers.clear();

		FLUX_VERIFY(s_Data->WhiteTexture->GetReferenceCount() == 1);
		FLUX_VERIFY(s_Data->BlackTexture->GetReferenceCount() == 1);

		FLUX_VERIFY(s_Data->FullscreenQuadVertexBuffer->GetReferenceCount() == 1);
		FLUX_VERIFY(s_Data->FullscreenQuadIndexBuffer->GetReferenceCount() == 1);

		delete s_Data;
		s_Data = nullptr;
	}

	void Renderer::BeginFrame()
	{
		FLUX_CHECK_IS_MAIN_THREAD();
		FLUX_ASSERT(!s_Data->ActivePipeline);
	}

	void Renderer::EndFrame()
	{
		FLUX_CHECK_IS_MAIN_THREAD();
		FLUX_ASSERT(!s_Data->ActivePipeline);

		s_Data->CurrentQueueIndex = (s_Data->CurrentQueueIndex + 1) % s_RenderCommandQueueCount;
		s_Data->CurrentFrameIndex = (s_Data->CurrentFrameIndex + 1) % Renderer::GetFramesInFlight();
		s_Data->FrameCount++;
	}

	void Renderer::BeginRenderPass(Ref<CommandBuffer> commandBuffer, Ref<GraphicsPipeline> pipeline)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		FLUX_ASSERT(!s_Data->ActivePipeline);
		FLUX_ASSERT(commandBuffer);
		FLUX_ASSERT(pipeline);

		s_Data->ActivePipeline = pipeline;

		pipeline->GetFramebuffer()->Bind(commandBuffer);
		pipeline->Bind(commandBuffer);
		pipeline->BindDescriptorSets(commandBuffer);
	}

	void Renderer::EndRenderPass(Ref<CommandBuffer> commandBuffer)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		FLUX_ASSERT(s_Data->ActivePipeline);
		FLUX_ASSERT(commandBuffer);

		s_Data->ActivePipeline->GetFramebuffer()->Unbind(commandBuffer);
		s_Data->ActivePipeline = nullptr;
	}

	void Renderer::RenderGeometry(Ref<CommandBuffer> commandBuffer, Ref<RenderMaterial> material, Ref<VertexBuffer> vertexBuffer, Ref<IndexBuffer> indexBuffer, uint32 indexCount)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		if (indexCount == 0)
			indexCount = indexBuffer->GetCount();

		vertexBuffer->Bind(commandBuffer);
		indexBuffer->Bind(commandBuffer);

		material->Bind(commandBuffer, s_Data->ActivePipeline);

		s_Data->ActivePipeline->DrawIndexed(commandBuffer, indexCount);
	}

	void Renderer::RenderFullscreenQuad(Ref<CommandBuffer> commandBuffer, Ref<RenderMaterial> material)
	{
		s_Data->FullscreenQuadVertexBuffer->Bind(commandBuffer);
		s_Data->FullscreenQuadIndexBuffer->Bind(commandBuffer);

		material->Bind(commandBuffer, s_Data->ActivePipeline);

		s_Data->ActivePipeline->DrawIndexed(commandBuffer, s_Data->FullscreenQuadIndexBuffer->GetCount());
	}

#ifndef FLUX_BUILD_SHIPPING
	void Renderer::SubmitRenderCommand(const char* functionName, RenderCommand command)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		if (s_RenderCommandQueueLocked[s_Data->CurrentQueueIndex])
		{
			FLUX_CRITICAL_CATEGORY("Renderer", "Recursive call from {0} detected!", functionName);
			FLUX_VERIFY(false);
		}

		s_RenderCommandQueue[s_Data->CurrentQueueIndex].push(std::move(command));
	}

	void Renderer::SubmitRenderCommandRelease(const char* functionName, RenderCommand command)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		uint32 frameIndex = Renderer::GetCurrentFrameIndex();

		if (s_ReleaseQueueLocked[frameIndex])
		{
			FLUX_CRITICAL_CATEGORY("Renderer", "Recursive call from {0} detected!", functionName);
			FLUX_VERIFY(false);
		}

		s_ReleaseQueue[frameIndex].push(std::move(command));
	}
#else
	void Renderer::SubmitRenderCommand(RenderCommand command)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		s_RenderCommandQueue[s_Data->CurrentQueueIndex].push(std::move(command));
	}

	void Renderer::SubmitRenderCommandRelease(RenderCommand command)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		uint32 frameIndex = Renderer::GetCurrentFrameIndex();
		s_ReleaseQueue[frameIndex].push(std::move(command));
	}
#endif

	void Renderer::RT_FlushRenderCommands(uint32 queueIndex)
	{
		FLUX_CHECK_IS_RENDER_THREAD();
		
#ifndef FLUX_BUILD_SHIPPING
		if (s_RenderCommandQueueLocked[queueIndex])
		{
			FLUX_CRITICAL_CATEGORY("Renderer", "RT_FlushRenderCommands called recursively!");
			FLUX_VERIFY(false);
		}

		s_RenderCommandQueueLocked[queueIndex] = true;
#endif

		auto& queue = s_RenderCommandQueue[queueIndex];
		while (!queue.empty())
		{
			RenderCommand& command = queue.front();
			command();
			queue.pop();
		}

#ifndef FLUX_BUILD_SHIPPING
		s_RenderCommandQueueLocked[queueIndex] = false;
#endif
	}

	void Renderer::RT_FlushReleaseQueue(uint32 frameIndex)
	{
		FLUX_CHECK_IS_RENDER_THREAD();

#ifndef FLUX_BUILD_SHIPPING
		if (s_ReleaseQueueLocked[frameIndex])
		{
			FLUX_CRITICAL_CATEGORY("Renderer", "RT_FlushReleaseQueue called recursively!");
			FLUX_VERIFY(false);
		}

		s_ReleaseQueueLocked[frameIndex] = true;
#endif

		auto& queue = s_ReleaseQueue[frameIndex];
		while (!queue.empty())
		{
			RenderCommand& command = queue.front();
			command();
			queue.pop();
		}

#ifndef FLUX_BUILD_SHIPPING
		s_ReleaseQueueLocked[frameIndex] = false;
#endif

	}

	void Renderer::RT_FlushReleaseQueues()
	{
		FLUX_CHECK_IS_RENDER_THREAD();

		for (uint32 frameIndex = 0; frameIndex < s_ReleaseQueueCount; frameIndex++)
			RT_FlushReleaseQueue(frameIndex);
	}

	uint32 Renderer::GetCurrentFrameIndex()
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		return s_Data->CurrentFrameIndex;
	}

	uint32 Renderer::RT_GetCurrentFrameIndex()
	{
		FLUX_CHECK_IS_RENDER_THREAD();

		return Engine::Get().GetSwapchain()->GetCurrentBufferIndex();
	}

	uint32 Renderer::GetCurrentQueueIndex()
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		return s_Data->CurrentQueueIndex;
	}

	uint32 Renderer::GetFramesInFlight()
	{
		return Engine::Get().GetSwapchain()->GetImageCount();
	}

	Ref<Texture2D> Renderer::GetWhiteTexture()
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		return s_Data->WhiteTexture;
	}

	Ref<Texture2D> Renderer::GetBlackTexture()
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		return s_Data->BlackTexture;
	}

	Ref<Shader> Renderer::GetShader(std::string_view name)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		auto it = s_Shaders.find(name);
		if (it != s_Shaders.end())
			return it->second;

		// TODO: shader registry

		std::filesystem::path path;
		path /= "Resources";
		path /= "Shaders";
		path /= std::string(name) + ".glsl";

		Ref<Shader> shader = Shader::Create(path);
		s_Shaders[name] = shader;
		return shader;
	}

	Ref<UniformBuffer> Renderer::GetUniformBuffer(std::string_view name)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		std::lock_guard<std::mutex> lock(s_UniformBufferMutex);

		for (const auto& entry : s_UniformBufferBindings)
		{
			if (entry.first == name)
			{
				auto bindingIt = s_UniformBuffers.find(entry.second);
				if (bindingIt != s_UniformBuffers.end())
				{
					auto it = bindingIt->second.find(Renderer::GetCurrentFrameIndex());
					if (it != bindingIt->second.end())
						return it->second;
				}
			}
		}
		return nullptr;
	}

	Ref<UniformBuffer> Renderer::RT_GetUniformBuffer(std::string_view name)
	{
		FLUX_CHECK_IS_RENDER_THREAD();

		std::lock_guard<std::mutex> lock(s_UniformBufferMutex);

		for (const auto& entry : s_UniformBufferBindings)
		{
			if (entry.first == name)
			{
				auto bindingIt = s_UniformBuffers.find(entry.second);
				if (bindingIt != s_UniformBuffers.end())
				{
					auto it = bindingIt->second.find(Renderer::RT_GetCurrentFrameIndex());
					if (it != bindingIt->second.end())
						return it->second;
				}
			}
		}
		return nullptr;
	}

	Ref<UniformBuffer> Renderer::GetUniformBuffer(std::string_view name, uint32 frameIndex)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		std::lock_guard<std::mutex> lock(s_UniformBufferMutex);

		for (const auto& entry : s_UniformBufferBindings)
		{
			if (entry.first == name)
			{
				auto bindingIt = s_UniformBuffers.find(entry.second);
				if (bindingIt != s_UniformBuffers.end())
				{
					auto it = bindingIt->second.find(frameIndex);
					if (it != bindingIt->second.end())
						return it->second;
				}
			}
		}
		return nullptr;
	}

	Ref<UniformBuffer> Renderer::RT_GetUniformBuffer(std::string_view name, uint32 frameIndex)
	{
		FLUX_CHECK_IS_RENDER_THREAD();

		std::lock_guard<std::mutex> lock(s_UniformBufferMutex);

		for (const auto& entry : s_UniformBufferBindings)
		{
			if (entry.first == name)
			{
				auto bindingIt = s_UniformBuffers.find(entry.second);
				if (bindingIt != s_UniformBuffers.end())
				{
					auto it = bindingIt->second.find(frameIndex);
					if (it != bindingIt->second.end())
						return it->second;
				}
			}
		}
		return nullptr;
	}

	bool Renderer::RegisterUniformBuffer(const ShaderUniformBuffer& buffer)
	{
		FLUX_CHECK_IS_MAIN_THREAD();

		std::lock_guard<std::mutex> lock(s_UniformBufferMutex);

		if (s_UniformBufferBindings.find(buffer.Name) != s_UniformBufferBindings.end())
			return false;

		if (s_UniformBuffers.find(buffer.Binding) != s_UniformBuffers.end())
			return false;

		uint32 framesInFlight = Renderer::GetFramesInFlight();

		UniformBufferCreateInfo createInfo = {};
		createInfo.Count = framesInFlight;
		createInfo.Size = buffer.Size;
		createInfo.Binding = buffer.Binding;

		for (const auto& [uniformName, uniform] : buffer.Uniforms)
		{
			createInfo.Uniforms[uniformName] = {
				uniformName,
				uniform.Size,
				uniform.Offset
			};
		}

		for (uint32 frameIndex = 0; frameIndex < framesInFlight; frameIndex++)
			s_UniformBuffers[buffer.Binding][frameIndex] = UniformBuffer::Create(createInfo);

		s_UniformBufferBindings[buffer.Name] = buffer.Binding;

		return true;
	}

}