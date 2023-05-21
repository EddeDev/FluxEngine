#pragma once

#include "Flux/Runtime/Core/Window.h"
#include "Flux/Runtime/Core/Thread.h"

#include "Flux/Runtime/Renderer/GraphicsAPI.h"
#include "Flux/Runtime/Renderer/GraphicsDevice.h"
#include "Flux/Runtime/Renderer/GraphicsContext.h"
#include "Flux/Runtime/Renderer/Swapchain.h"
#include "Flux/Runtime/Renderer/CommandBuffer.h"
#include "Flux/Runtime/Renderer/Framebuffer.h"
#include "Flux/Runtime/Renderer/GraphicsPipeline.h"
#include "Flux/Runtime/Renderer/Shader.h"
#include "Flux/Runtime/Renderer/VertexBuffer.h"
#include "Flux/Runtime/Renderer/IndexBuffer.h"

namespace Flux {

	class Engine
	{
	protected:
		Engine();
	public:
		~Engine();

		void Run();
		void Close(bool restart = false);
		void SubmitToEventThread(std::function<void()> function);
		void SubmitToMainThread(std::function<void()> function);

		void SetGraphicsAPI(GraphicsAPI api) { m_GraphicsAPI = api; }
		GraphicsAPI GetGraphicsAPI() const { return m_GraphicsAPI; }

		Ref<GraphicsContext> GetGraphicsContext() const { return m_Context; }
		Ref<GraphicsAdapter> GetGraphicsAdapter() const { return m_Adapter; }
		Ref<GraphicsDevice> GetGraphicsDevice() const { return m_Device; }
		Ref<Swapchain> GetSwapchain() const { return m_Swapchain; }

		ThreadID GetMainThreadID() const { return m_MainThreadID; }
		ThreadID GetEventThreadID() const { return m_EventThreadID; }

		Unique<Window>& GetWindow() { return m_Window; }
		const Unique<Window>& GetWindow() const { return m_Window; }

		template<typename TEngine = Engine>
		static TEngine& Get()
		{
			FLUX_ASSERT(s_Instance, "Engine instance is nullptr!");
			return (TEngine&)*s_Instance;
		}
	private:
		void MT_MainLoop();

		void OnWindowClose();
		void OnWindowResize(uint32 width, uint32 height);
	protected:
		virtual void OnInit() {}
		virtual void OnExit() {}
		virtual void OnUpdate() {}
 	protected:
		inline static Engine* s_Instance = nullptr;

		Unique<Window> m_Window;
		Unique<Thread> m_MainThread;

		GraphicsAPI m_GraphicsAPI = GraphicsAPI::Vulkan;

		Ref<GraphicsContext> m_Context;
		Ref<GraphicsAdapter> m_Adapter;
		Ref<GraphicsDevice> m_Device;
		Ref<Swapchain> m_Swapchain;

		Ref<CommandBuffer> m_SwapchainCommandBuffer;
		Ref<Framebuffer> m_SwapchainFramebuffer;
		Ref<GraphicsPipeline> m_Pipeline;
		Ref<Shader> m_Shader;
		Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer> m_IndexBuffer;

		ThreadID m_EventThreadID;
		std::queue<std::function<void()>> m_EventThreadQueue;
		std::mutex m_EventThreadMutex;

		ThreadID m_MainThreadID;
		std::queue<std::function<void()>> m_MainThreadQueue;
		std::mutex m_MainThreadMutex;

		std::atomic<bool> m_Running = true;
		std::atomic<bool> m_Minimized = false;
		
		float m_FrameTime = 0.0f;
		float m_LastFrameTime = 0.0f;

		float m_LastTime = 0.0f;
		uint32 m_FrameCounter = 0;
		uint32 m_FramesPerSecond = 0;
	};

#ifdef FLUX_ENABLE_ASSERTS
	#define FLUX_ASSERT_ON_MAIN_THREAD() FLUX_ASSERT_ON_THREAD(Engine::Get().GetMainThreadID())
	#define FLUX_ASSERT_ON_EVENT_THREAD() FLUX_ASSERT_ON_THREAD(Engine::Get().GetEventThreadID())
#else
	#define FLUX_ASSERT_ON_MAIN_THREAD() (void)0
	#define FLUX_ASSERT_ON_EVENT_THREAD() (void)0
#endif

#ifndef FLUX_BUILD_SHIPPING
	#define FLUX_VERIFY_ON_MAIN_THREAD() FLUX_ASSERT_ON_THREAD(Engine::Get().GetMainThreadID())
	#define FLUX_VERIFY_ON_EVENT_THREAD() FLUX_ASSERT_ON_THREAD(Engine::Get().GetEventThreadID())
#else
	#define FLUX_VERIFY_ON_MAIN_THREAD() (void)0
	#define FLUX_VERIFY_ON_EVENT_THREAD() (void)0
#endif

#define FLUX_CURRENT_GRAPHICS_API Engine::Get().GetGraphicsAPI()

}