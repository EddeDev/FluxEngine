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
		void OnWindowMinimize(bool minimized);
 	protected:
		inline static Engine* s_Instance = nullptr;

		Unique<Window> m_Window;
		Unique<Thread> m_MainThread;

		Application* m_Application = nullptr;

		GraphicsAPI m_GraphicsAPI = GraphicsAPI::Vulkan;

		Ref<GraphicsContext> m_Context;
		Ref<GraphicsAdapter> m_Adapter;
		Ref<GraphicsDevice> m_Device;
		Ref<Swapchain> m_Swapchain;

		ThreadID m_EventThreadID;
		std::queue<std::function<void()>> m_EventThreadQueue;
		std::mutex m_EventThreadMutex;

		ThreadID m_MainThreadID;
		std::queue<std::function<void()>> m_MainThreadQueue;
		std::mutex m_MainThreadMutex;

		std::atomic<bool> m_Running = false;
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