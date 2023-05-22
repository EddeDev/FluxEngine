#include "FluxPCH.h"
#include "Engine.h"

#include "Flux/Runtime/Core/Platform.h"
#include "Flux/Runtime/Core/Window.h"

#include "Flux/Runtime/Renderer/Renderer.h"

namespace Flux {

	namespace Utils {

		static void ExecuteQueue(std::queue<std::function<void()>>& queue, std::mutex& mutex)
		{
			std::unique_lock<std::mutex> lock(mutex);
			while (!queue.empty())
			{
				auto& callback = queue.front();
				callback();
				queue.pop();
			}
		}
	}

	extern bool g_EngineRunning;

	Engine::Engine()
	{
		FLUX_ASSERT(!s_Instance, "Engine instance already exists!");
		s_Instance = this;

		m_EventThreadID = Platform::GetCurrentThreadID();

		Platform::SetConsoleTitle("Flux Engine");
		Platform::SetThreadName(Platform::GetCurrentThread(), "Event Thread");
		Platform::SetThreadPriority(Platform::GetCurrentThread(), ThreadPriority::Lowest);

		WindowCreateInfo windowCreateInfo;
		windowCreateInfo.Width = 1280;
		windowCreateInfo.Height = 720;

		m_Window = Window::Create(windowCreateInfo);
		m_Window->AddCloseCallback(FLUX_BIND_CALLBACK(OnWindowClose, this));
		m_Window->AddSizeCallback(FLUX_BIND_CALLBACK(OnWindowResize, this));

		FLUX_INFO("Graphics API: {0}", GraphicsAPIUtils::ToString(m_GraphicsAPI));
	}

	Engine::~Engine()
	{
		FLUX_ASSERT_ON_EVENT_THREAD();

		s_Instance = nullptr;
	}

	void Engine::Run()
	{
		FLUX_ASSERT_ON_EVENT_THREAD();

		ThreadCreateInfo mainThreadCreateInfo;
		mainThreadCreateInfo.Name = "Main Thread";
		mainThreadCreateInfo.Priority = ThreadPriority::Highest;

		m_MainThread = Thread::Create(mainThreadCreateInfo);
		m_MainThreadID = m_MainThread->GetID();

		m_MainThread->Submit(FLUX_BIND_CALLBACK(MT_MainLoop, this));

		while (m_Running)
		{
			Platform::WaitMessage();
			Platform::PumpMessages();

			Utils::ExecuteQueue(m_EventThreadQueue, m_EventThreadMutex);
		}

		m_MainThread.reset();
	}

	void Engine::MT_MainLoop()
	{
		FLUX_ASSERT_ON_MAIN_THREAD();

		m_Context = GraphicsContext::Create();
		m_Adapter = GraphicsAdapter::Create(m_Context);
		m_Device = GraphicsDevice::Create(m_Adapter);
		m_Swapchain = Swapchain::Create(m_Window.get());

		Renderer::Init();

		{
			CommandBufferCreateInfo commandBufferCreateInfo;
			commandBufferCreateInfo.CreateFromSwapchain = true;
			m_SwapchainCommandBuffer = CommandBuffer::Create(commandBufferCreateInfo);

			FramebufferCreateInfo framebufferCreateInfo;
			framebufferCreateInfo.ClearColor = { 0.1f, 0.1f, 0.1f, 1.0f };
			framebufferCreateInfo.SwapchainTarget = true;
			m_SwapchainFramebuffer = Framebuffer::Create(framebufferCreateInfo);

			m_Shader = Shader::Create("Resources/Shaders/Shader.glsl");

			GraphicsPipelineCreateInfo pipelineCreateInfo;
			pipelineCreateInfo.Shader = m_Shader;
			pipelineCreateInfo.Framebuffer = m_SwapchainFramebuffer;
			m_Pipeline = GraphicsPipeline::Create(pipelineCreateInfo);

			struct Vertex
			{
				glm::vec3 Position;
				glm::vec4 Color;
			};

			Vertex vertices[4];

			vertices[0].Position = { -0.5f, -0.5f, 0.0f };
			vertices[0].Color = { 0.8f, 0.2f, 0.2f, 1.0f };

			vertices[1].Position = { 0.5f, -0.5f, 0.0f };
			vertices[1].Color = { 0.2f, 0.8f, 0.2f, 1.0f };

			vertices[2].Position = { 0.5f, 0.5f, 0.0f };
			vertices[2].Color = { 0.2f, 0.2f, 0.8f, 1.0f };

			vertices[3].Position = { -0.5f, 0.5f, 0.0f };
			vertices[3].Color = { 0.2f, 0.2f, 0.8f, 1.0f };

			uint32 indices[6] = {
				0, 1, 2,
				2, 3, 0
			};

			m_VertexBuffer = VertexBuffer::Create(vertices, sizeof(Vertex) * 4);
			m_IndexBuffer = IndexBuffer::Create(indices, sizeof(uint32) * 6);
		}
	
		OnInit();

		SubmitToEventThread([this]()
		{
			m_Window->SetVisible(true);
		});

		while (m_Running)
		{
			float time = Platform::GetTime();
			m_FrameTime = time - m_LastFrameTime;
			m_LastFrameTime = time;

			m_FrameCounter++;
			if (time >= m_LastTime + 1.0f)
			{
				FLUX_TRACE("Frame time: {0:.2f}ms ({1} fps)", m_FrameTime * 1000.0f, m_FrameCounter);

				m_FramesPerSecond = m_FrameCounter;
				m_FrameCounter = 0;
				m_LastTime = time;
			}

			Utils::ExecuteQueue(m_MainThreadQueue, m_MainThreadMutex);

			if (!m_Minimized)
			{
				Renderer::FlushReleaseQueue(Renderer::GetCurrentFrameIndex());

				m_Swapchain->BeginFrame();

				Renderer::BeginFrame();

				OnUpdate();

				{
					m_SwapchainCommandBuffer->Begin();
					Renderer::BeginRenderPass(m_SwapchainCommandBuffer, m_SwapchainFramebuffer);

					m_VertexBuffer->Bind(m_SwapchainCommandBuffer);
					m_Pipeline->Bind(m_SwapchainCommandBuffer);
					m_IndexBuffer->Bind(m_SwapchainCommandBuffer);

					m_Pipeline->DrawIndexed(m_SwapchainCommandBuffer, m_IndexBuffer->GetCount());

					Renderer::EndRenderPass(m_SwapchainCommandBuffer);
					m_SwapchainCommandBuffer->End();
				}

				Renderer::FlushRenderCommands();
				Renderer::EndFrame();

				m_Swapchain->Present(1);
			}
		}

		OnExit();

		{
			FLUX_VERIFY(m_VertexBuffer->GetReferenceCount() == 1);
			m_VertexBuffer = nullptr;

			FLUX_VERIFY(m_IndexBuffer->GetReferenceCount() == 1);
			m_IndexBuffer = nullptr;

			FLUX_VERIFY(m_Pipeline->GetReferenceCount() == 1);
			m_Pipeline = nullptr;

			FLUX_VERIFY(m_Shader->GetReferenceCount() == 1);
			m_Shader = nullptr;

			FLUX_VERIFY(m_SwapchainFramebuffer->GetReferenceCount() == 1);
			m_SwapchainFramebuffer = nullptr;

			FLUX_VERIFY(m_SwapchainCommandBuffer->GetReferenceCount() == 1);
			m_SwapchainCommandBuffer = nullptr;
		}

		Renderer::Shutdown();

		FLUX_VERIFY(m_Swapchain->GetReferenceCount() == 1);
		m_Swapchain = nullptr;

		FLUX_VERIFY(m_Device->GetReferenceCount() == 1);
		m_Device = nullptr;

		FLUX_VERIFY(m_Adapter->GetReferenceCount() == 1);
		m_Adapter = nullptr;

		FLUX_VERIFY(m_Context->GetReferenceCount() == 1);
		m_Context = nullptr;
	}

	void Engine::Close(bool restart)
	{
		FLUX_ASSERT_ON_MAIN_THREAD();

		m_Running = false;
		g_EngineRunning = restart;
	}

	void Engine::OnWindowClose()
	{
		FLUX_ASSERT_ON_EVENT_THREAD();

		SubmitToMainThread([this]()
		{
			Close();
		});
	}

	void Engine::OnWindowResize(uint32 width, uint32 height)
	{
		FLUX_ASSERT_ON_EVENT_THREAD();

		if (width == 0 || height == 0)
		{
			m_Minimized = true;
			return;
		}

		m_Minimized = false;
	}

	void Engine::SubmitToEventThread(std::function<void()> function)
	{
		std::lock_guard<std::mutex> lock(m_EventThreadMutex);
		m_EventThreadQueue.push(std::move(function));
		Platform::PostEmptyEvent();
	}

	void Engine::SubmitToMainThread(std::function<void()> function)
	{
		std::lock_guard<std::mutex> lock(m_MainThreadMutex);
		m_MainThreadQueue.push(std::move(function));
	}

}