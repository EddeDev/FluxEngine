#pragma once

#include "Window.h"
#include "Thread.h"

#include "Flux/Renderer/GraphicsAPI.h"
#include "Flux/Renderer/GraphicsContext.h"
#include "Flux/Renderer/ImGuiRenderer.h"
#include "Flux/Renderer/VertexBuffer.h"
#include "Flux/Renderer/IndexBuffer.h"
#include "Flux/Renderer/Shader.h"
#include "Flux/Renderer/GraphicsPipeline.h"
#include "Flux/Renderer/Texture.h"

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
		
		float GetFrameTime() const { return m_FrameTime; }

		const Unique<Window>& GetWindow() const { return m_Window; }

		GraphicsAPI GetGraphicsAPI() const { return m_GraphicsAPI; }

		ThreadID GetMainThreadID() const { return m_MainThreadID; }
		ThreadID GetEventThreadID() const { return m_EventThreadID; }
		ThreadID GetRenderThreadID() const { return m_RenderThreadID; }

		static Engine& Get() { return *(Engine*)s_Instance; }
	private:
		void MainLoop();

		void OnWindowClose();
		void OnMenuCallback(WindowMenu menu, uint32 menuID);
	private:
		inline static Engine* s_Instance = nullptr;

		Unique<Window> m_Window;

		Ref<GraphicsContext> m_Context;
		Ref<ImGuiRenderer> m_ImGuiRenderer;

		GraphicsAPI m_GraphicsAPI = GraphicsAPI::OpenGL;

		Unique<Thread> m_MainThread;
		Unique<Thread> m_RenderThread;

		Ref<Texture2D> m_ViewportPlaceholderTexture;
		uint8* m_ViewportPlaceholderTextureData = nullptr;
		uint32 m_ViewportWidth = 0;
		uint32 m_ViewportHeight = 0;

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
		float m_FrameTime = 0.0f;
		float m_LastFrameTime = 0.0f;

		float m_Time = 0.0f;
		float m_LastTime = 0.0f;
		uint32 m_FrameCounter = 0;
		uint32 m_FramesPerSecond = 0;

		float m_RenderThreadWaitTime = 0.0f;

		enum MenuItem : uint32
		{
			// File
			Menu_File_NewProject,
			Menu_File_OpenProject,
			Menu_File_SaveProject,
			Menu_File_Restart,
			Menu_File_Exit,

			// Edit
			Menu_Edit_Preferences,

			// About
			Menu_About_AboutFluxEngine
		};

		std::unordered_map<MenuItem, Unique<Window>> m_Windows;
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