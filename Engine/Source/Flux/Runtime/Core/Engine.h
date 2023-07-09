#pragma once

#include "Application.h"
#include "Window.h"
#include "Thread.h"

#include "Flux/Runtime/Renderer/GraphicsAPI.h"
#include "Flux/Runtime/Renderer/GraphicsDevice.h"
#include "Flux/Runtime/Renderer/GraphicsContext.h"
#include "Flux/Runtime/Renderer/Swapchain.h"

namespace Flux {

	class Engine
	{
	public:
		Engine();
		~Engine();

		void Run();
		void Close(bool restart = false);
		void SubmitToEventThread(std::function<void()> function);
		void SubmitToMainThread(std::function<void()> function);

		template<typename TApplication, typename... TArgs>
		void SetApplication(TArgs&&... args)
		{
			static_assert(std::is_base_of<Application, TApplication>::value);
			FLUX_VERIFY(!m_Application, "Application already exists!");
			m_Application = new TApplication(std::forward<TArgs>(args)...);
		}

		float GetTime() const { return m_Time; }
		float GetFrameTime() const { return m_FrameTime; }
		uint32 GetFramesPerSecond() const { return m_FramesPerSecond; }

		void SetGraphicsAPI(GraphicsAPI api) { m_GraphicsAPI = api; }
		GraphicsAPI GetGraphicsAPI() const { return m_GraphicsAPI; }

		Ref<GraphicsContext> GetGraphicsContext() const { return m_Context; }
		Ref<GraphicsAdapter> GetGraphicsAdapter() const { return m_Adapter; }
		Ref<GraphicsDevice> GetGraphicsDevice() const { return m_Device; }
		Ref<Swapchain> GetSwapchain() const { return m_Swapchain; }

		ThreadID GetMainThreadID() const { return m_MainThreadID; }
		ThreadID GetEventThreadID() const { return m_EventThreadID; }
		ThreadID GetRenderThreadID() const { return m_RenderThreadID; }

		Unique<Window>& GetWindow() { return m_Window; }
		const Unique<Window>& GetWindow() const { return m_Window; }

		template<typename TEngine = Engine>
		static TEngine& Get()
		{
			FLUX_ASSERT(s_Instance, "Engine instance is nullptr!");
			return (TEngine&)*s_Instance;
		}
	private:
		void RT_Initialize();
		void RT_Shutdown();
		void MT_MainLoop();
		void MT_UpdateAndRender();

		void OnWindowClose();
		void OnWindowResize(uint32 width, uint32 height);
		void OnWindowMinimize(bool minimized);
 	protected:
		inline static Engine* s_Instance = nullptr;

		Unique<Window> m_Window;
		Application* m_Application = nullptr;
		GraphicsAPI m_GraphicsAPI = GraphicsAPI::Vulkan;

		Ref<GraphicsContext> m_Context;
		Ref<GraphicsAdapter> m_Adapter;
		Ref<GraphicsDevice> m_Device;
		Ref<Swapchain> m_Swapchain;

		Unique<Thread> m_MainThread;
		Unique<Thread> m_RenderThread;

		ThreadID m_EventThreadID;
		ThreadID m_MainThreadID;
		ThreadID m_RenderThreadID;

		std::queue<std::function<void()>> m_EventThreadQueue;
		std::mutex m_EventThreadMutex;

		std::queue<std::function<void()>> m_MainThreadQueue;
		std::mutex m_MainThreadMutex;

		std::atomic<bool> m_Running = false;
		std::atomic<bool> m_Minimized = false;

		bool m_VSync = false;
		float m_FrameTime = 0.0f;
		float m_LastFrameTime = 0.0f;

		float m_Time = 0.0f;
		float m_LastTime = 0.0f;
		uint32 m_FrameCounter = 0;
		uint32 m_FramesPerSecond = 0;

		struct  
		{
			float ApplicationUpdate = 0.0f;
			float WaitForRenderThread = 0.0f;
		} m_PerformanceTimers;
	};

#ifndef FLUX_BUILD_SHIPPING
	#define FLUX_CHECK_IS_MAIN_THREAD() FLUX_VERIFY_IS_THREAD(Engine::Get().GetMainThreadID())
	#define FLUX_CHECK_IS_EVENT_THREAD() FLUX_VERIFY_IS_THREAD(Engine::Get().GetEventThreadID())
	#define FLUX_CHECK_IS_RENDER_THREAD() FLUX_VERIFY_IS_THREAD(Engine::Get().GetRenderThreadID())
#else
	#define FLUX_CHECK_IS_MAIN_THREAD() (void)0
	#define FLUX_CHECK_IS_EVENT_THREAD() (void)0
	#define FLUX_CHECK_IS_RENDER_THREAD() (void)0
#endif

#define FLUX_CURRENT_GRAPHICS_API Engine::Get().GetGraphicsAPI()

}