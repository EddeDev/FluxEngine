#pragma once

#include "Window.h"
#include "Thread.h"
#include "BuildConfiguration.h"

#include "Events/WindowEvent.h"

#include "Flux/Runtime/Renderer/GraphicsAPI.h"
#include "Flux/Runtime/Renderer/GraphicsContext.h"
#include "Flux/Runtime/Renderer/ImGuiRenderer.h"

namespace Flux {

	struct EngineCreateInfo
	{
		std::string Title = "Flux Engine";
		bool EnableImGui = true;
		bool ShowSplashScreen = true;
		bool MaximizeOnStart = false;
		bool Multithreaded = true;
	};

	class Engine
	{
	public:
		Engine(const EngineCreateInfo& createInfo);
		virtual ~Engine();

		void Run();
		void Close(bool restart = false);
		void SubmitToEventThread(std::function<void()> function);
		void SubmitToMainThread(std::function<void()> function);

		float GetDeltaTime() const { return m_DeltaTime; }

		Ref<Window> GetMainWindow() const { return m_MainWindow; }

		GraphicsAPI GetGraphicsAPI() const { return m_GraphicsAPI; }

		ThreadID GetMainThreadID() const { return m_MainThreadID; }
		ThreadID GetEventThreadID() const { return m_EventThreadID; }
		ThreadID GetRenderThreadID() const { return m_RenderThreadID; }

		static BuildConfiguration GetBuildConfiguration();

		static Engine& Get() { return *(Engine*)s_Instance; }
	protected:
		virtual void OnInit() {}
		virtual void OnShutdown() {}
		virtual void OnUpdate(float deltaTime) {}
		virtual void OnImGuiRender() {}
	private:
		void CreateRendererContext();
		void DestroyRendererContext();

		void MainLoop();

		void OnWindowCloseEvent(WindowCloseEvent& e);
	protected:
		inline static Engine* s_Instance = nullptr;

		EngineCreateInfo m_CreateInfo;

		Ref<Window> m_MainWindow;

		Ref<GraphicsContext> m_Context;
		Ref<ImGuiRenderer> m_ImGuiRenderer;

		GraphicsAPI m_GraphicsAPI = GraphicsAPI::OpenGL;

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
		bool m_RestartOnClose = false;

		bool m_VSync = true;
		float m_DeltaTime = 0.0f;
		float m_FixedDeltaTime = 1.0f / 50.0f;
		float m_MaxDeltaTime = 1.0f / 30.0f;
		float m_CurrentTime = 0.0f;
		float m_LastTime = 0.0f;

		float m_LastFrameTime = 0.0f;
		uint32 m_FrameCounter = 0;
		uint32 m_FramesPerSecond = 0;

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