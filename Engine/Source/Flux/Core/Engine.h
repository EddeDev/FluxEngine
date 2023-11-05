#pragma once

#include "Window.h"
#include "Thread.h"

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
		
		ThreadID GetMainThreadID() const { return m_MainThreadID; }
		ThreadID GetEventThreadID() const { return m_EventThreadID; }
		ThreadID GetRenderThreadID() const { return m_RenderThreadID; }

		static Engine& Get() { return *(Engine*)s_Instance; }
	private:
		void MainLoop();
	private:
		inline static Engine* s_Instance = nullptr;

		Unique<Window> m_Window;

		Unique<Thread> m_MainThread;
		Unique<Thread> m_RenderThread;

		ThreadID m_EventThreadID = 0;
		ThreadID m_MainThreadID = 0;
		ThreadID m_RenderThreadID = 0;

		std::queue<std::function<void()>> m_EventThreadQueue;
		std::mutex m_EventThreadMutex;

		std::queue<std::function<void()>> m_MainThreadQueue;
		std::mutex m_MainThreadMutex;

		std::atomic<bool> m_Running = true;

		bool m_VSync = false;
		float m_FrameTime = 0.0f;
		float m_LastFrameTime = 0.0f;

		float m_Time = 0.0f;
		float m_LastTime = 0.0f;
		uint32 m_FrameCounter = 0;
		uint32 m_FramesPerSecond = 0;
	};

#ifndef FLUX_BUILD_SHIPPING
	#define FLUX_CHECK_IS_MAIN_THREAD() FLUX_CHECK_IS_THREAD(Engine::Get().GetMainThreadID())
	#define FLUX_CHECK_IS_EVENT_THREAD() FLUX_CHECK_IS_THREAD(Engine::Get().GetEventThreadID())
	#define FLUX_CHECK_IS_RENDER_THREAD() FLUX_CHECK_IS_THREAD(Engine::Get().GetRenderThreadID())
#else
	#define FLUX_CHECK_IS_MAIN_THREAD() (void)0
	#define FLUX_CHECK_IS_EVENT_THREAD() (void)0
	#define FLUX_CHECK_IS_RENDER_THREAD() (void)0
#endif

}