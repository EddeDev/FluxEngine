#pragma once

#include "ResourceAllocator.h"
#include "CommandBuffer.h"
#include "Framebuffer.h"

#include "GraphicsPipeline.h"
#include "Shader.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "UniformBuffer.h"
#include "Texture.h"

namespace Flux {

	using RenderCommand = std::function<void()>;

	class Renderer
	{
	public:
		static void Init();
		static void Shutdown();
		static void DestroyResources();

		static void BeginFrame();
		static void EndFrame();

		static void BeginRenderPass(Ref<CommandBuffer> commandBuffer, Ref<GraphicsPipeline> pipeline);
		static void EndRenderPass(Ref<CommandBuffer> commandBuffer);
		
		static void RenderGeometry(Ref<CommandBuffer> commandBuffer, Ref<VertexBuffer> vertexBuffer, Ref<IndexBuffer> indexBuffer, uint32 indexCount = 0);

#ifndef FLUX_BUILD_SHIPPING
		static void SubmitRenderCommand(const char* functionName, RenderCommand command);
		static void SubmitRenderCommandRelease(const char* functionName, RenderCommand command);
#else
		static void SubmitRenderCommand(RenderCommand command);
		static void SubmitRenderCommandRelease(RenderCommand command);
#endif
		static void RT_FlushRenderCommands(uint32 queueIndex);
		static void RT_FlushReleaseQueue(uint32 frameIndex);
		static void RT_FlushReleaseQueues();

		static uint32 GetCurrentFrameIndex();
		static uint32 RT_GetCurrentFrameIndex();
		static uint32 GetCurrentQueueIndex();
		static uint32 GetFramesInFlight();

		static Ref<Texture2D> GetWhiteTexture();
		static Ref<Texture2D> GetBlackTexture();

		static Ref<UniformBuffer> GetUniformBuffer(std::string_view name);
		static std::vector<Ref<UniformBuffer>> GetUniformBufferSet(std::string_view name);
		static bool RegisterUniformBuffer(const ShaderUniformBuffer& buffer);

		template<typename T = ResourceAllocator>
		static T& GetResourceAllocator()
		{
			static_assert(std::is_base_of<ResourceAllocator, T>::value);

			FLUX_ASSERT(s_ResourceAllocator, "Resource allocator is NULL");
			return *(T*)s_ResourceAllocator;
		}
	private:
		inline static constexpr uint32 s_RenderCommandQueueCount = 2;
		inline static constexpr uint32 s_ReleaseQueueCount = 3;

		inline static std::unordered_map<uint32, Ref<UniformBuffer>> s_UniformBuffers;
		inline static std::unordered_map<std::string, uint32> s_UniformBufferBindings;

		inline static std::queue<RenderCommand> s_RenderCommandQueue[s_RenderCommandQueueCount];
		inline static std::queue<RenderCommand> s_ReleaseQueue[s_ReleaseQueueCount];

#ifndef FLUX_BUILD_SHIPPING
		inline static std::atomic<bool> s_RenderCommandQueueLocked[s_RenderCommandQueueCount];
		inline static std::atomic<bool> s_ReleaseQueueLocked[s_ReleaseQueueCount];
#endif

		inline static ResourceAllocator* s_ResourceAllocator = nullptr;
	};

#ifndef FLUX_BUILD_SHIPPING
	#define FLUX_SUBMIT_RENDER_COMMAND(...) Renderer::SubmitRenderCommand(__FUNCTION__, __VA_ARGS__);
#else
	#define FLUX_SUBMIT_RENDER_COMMAND(...) Renderer::SubmitRenderCommand(__VA_ARGS__);
#endif

#ifndef FLUX_BUILD_SHIPPING
	#define FLUX_SUBMIT_RENDER_COMMAND_RELEASE(...) Renderer::SubmitRenderCommandRelease(__FUNCTION__, __VA_ARGS__);
#else
	#define FLUX_SUBMIT_RENDER_COMMAND_RELEASE(...) Renderer::SubmitRenderCommandRelease(__VA_ARGS__);
#endif

}