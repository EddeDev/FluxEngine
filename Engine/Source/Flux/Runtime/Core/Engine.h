#pragma once

#include "Window.h"
#include "Thread.h"
#include "BuildConfiguration.h"

#include "Events/EventQueue.h"

#include "Flux/Runtime/Renderer/GraphicsAPI.h"
#include "Flux/Runtime/Renderer/GraphicsContext.h"
#include "Flux/Runtime/Renderer/Framebuffer.h"

#include "Flux/Runtime/ImGui/ImGuiRenderer.h"

namespace Flux {

	struct EngineCreateInfo
	{
		std::string Title = "Flux Engine";
		uint32 WindowWidth = 1280;
		uint32 WindowHeight = 720;
		bool EnableImGui = true;
		bool ClearImGuiPass = false;
		bool ShowSplashScreen = true;
		bool MaximizeOnStart = false;
		bool Multithreaded = true;
		bool VSync = true;
	};

	class Engine
	{
	public:
		Engine(const EngineCreateInfo& createInfo);
		virtual ~Engine();

		void Run();
		void Close(bool restart = false);
		void SubmitToEventThread(std::function<void()> function, bool wait);
		void SubmitToMainThread(std::function<void()> function);

		template<bool TWait = false>
		void SubmitToEventThread(std::function<void()> function)
		{
			if constexpr (TWait)
				SubmitToEventThread(std::move(function), true);
			else
				SubmitToEventThread(std::move(function), false);
		}

		float GetTime() const { return m_CurrentTime; }
		float GetDeltaTime() const { return m_DeltaTime; }

		Ref<Window> GetMainWindow() const { return m_MainWindow; }
		Ref<ImGuiRenderer> GetImGuiRenderer() const { return m_ImGuiRenderer; }

		GraphicsAPI GetGraphicsAPI() const { return m_GraphicsAPI; }

		ThreadID GetMainThreadID() const { return m_MainThreadID; }
		ThreadID GetEventThreadID() const { return m_EventThreadID; }
		ThreadID GetRenderThreadID() const { return m_RenderThreadID; }

		static BuildConfiguration GetBuildConfiguration();

		static Engine& Get() { return *s_Instance; }
	protected:
		virtual void OnInit() {}
		virtual void OnShutdown() {}
		virtual void OnUpdate() {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& event) {}
	private:
		void CreateRendererContext();
		void DestroyRendererContext();

		void MainLoop();
	protected:
		inline static Engine* s_Instance = nullptr;

		EngineCreateInfo m_CreateInfo;

		Ref<Window> m_SplashScreenWindow;
		Ref<Window> m_MainWindow;

		Ref<EventQueue> m_EventQueue;

		Ref<GraphicsContext> m_Context;
		Ref<Framebuffer> m_SwapchainFramebuffer;
		Ref<Framebuffer> m_ImGuiFramebuffer;
		Ref<ImGuiRenderer> m_ImGuiRenderer;

		GraphicsAPI m_GraphicsAPI = GraphicsAPI::OpenGL;

		Unique<Thread> m_MainThread;
		Unique<Thread> m_RenderThread;

		ThreadID m_EventThreadID = 0;
		ThreadID m_MainThreadID = 0;
		ThreadID m_RenderThreadID = 0;

		std::queue<std::function<void()>> m_EventThreadQueue;
		std::condition_variable m_EventThreadCondVar;
		std::mutex m_EventThreadMutex;

		std::queue<std::function<void()>> m_MainThreadQueue;
		std::mutex m_MainThreadMutex;

		std::atomic<bool> m_Running = true;
		bool m_Minimized = false;
		bool m_RestartOnClose = false;

		bool m_VSync = true;
		float m_DeltaTime = 0.0f;
		float m_FixedDeltaTime = 1.0f / 50.0f;
		float m_MaxDeltaTime = 1.0f / 30.0f;
		float m_CurrentTime = 0.0f;
		float m_Accumulator = 0.0f;
		float m_LastTime = 0.0f;

		float m_LastFrameTime = 0.0f;
		uint32 m_FrameCounter = 0;
		uint32 m_TickCounter = 0;
		uint32 m_EventCounter = 0;
		uint32 m_FramesPerSecond = 0;
		uint32 m_TicksPerSecond = 0;
		uint32 m_EventsPerSecond = 0;

		float m_RenderThreadWaitTime = 0.0f;
	};

#ifndef FLUX_BUILD_SHIPPING
	#define FLUX_CHECK_IS_IN_MAIN_THREAD() FLUX_CHECK_IS_IN_THREAD(Engine::Get().GetMainThreadID())
	#define FLUX_CHECK_IS_IN_EVENT_THREAD() FLUX_CHECK_IS_IN_THREAD(Engine::Get().GetEventThreadID())
	#define FLUX_CHECK_IS_IN_RENDER_THREAD() FLUX_CHECK_IS_IN_THREAD(Engine::Get().GetRenderThreadID())
#else
	#define FLUX_CHECK_IS_IN_MAIN_THREAD() (void)0
	#define FLUX_CHECK_IS_IN_EVENT_THREAD() (void)0
	#define FLUX_CHECK_IS_IN_RENDER_THREAD() (void)0
#endif

}