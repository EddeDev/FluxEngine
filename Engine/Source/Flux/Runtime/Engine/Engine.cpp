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

			const uint32 maxQuads = 100;
			const uint32 maxQuadVertices = 100 * 4;
			const uint32 maxQuadIndices = 100 * 6;

			Vertex* vertices = new Vertex[maxQuadVertices];
			memset(vertices, 0, sizeof(Vertex) * maxQuadVertices);

			Vertex* vertexPtr = vertices;

			m_QuadIndexCount = 0;

			auto addRect = [&](float minX, float minY, float maxX, float maxY, const glm::vec4& color = glm::vec4(1.0f))
			{
				vertexPtr->Position = { minX, minY, 0.0f };
				vertexPtr->Color = color;
				vertexPtr++;

				vertexPtr->Position = { maxX, minY, 0.0f };
				vertexPtr->Color = color;
				vertexPtr++;

				vertexPtr->Position = { maxX, maxY, 0.0f };
				vertexPtr->Color = color;
				vertexPtr++;

				vertexPtr->Position = { minX, maxY, 0.0f };
				vertexPtr->Color = color;
				vertexPtr++;

				m_QuadIndexCount += 6;
			};

			const float xPadding = 8.0f;
			const float yPadding = 6.0f;
			const float spacing = 6.0f;

			const float windowWidth = 1280.0f;
			const float windowHeight = 720.0f;

			float xs = 57.0f;
			float ys = 18.0f;
			float menuBarHeight = 20.0f;
			addRect(
				xPadding,
				windowHeight - menuBarHeight - ys - yPadding, 
				xs + xPadding, 
				windowHeight - menuBarHeight - yPadding,
				{ 0.22f, 0.22f, 0.22f, 1.0f }
			);

			float xs2 = 32.0f;
			addRect(
				xPadding + xs + spacing,
				windowHeight - menuBarHeight - ys - yPadding,
				xPadding + xs + spacing + xs2,
				windowHeight - menuBarHeight - yPadding,
				{ 0.22f, 0.22f, 0.22f, 1.0f }
			);
			
			addRect(50.0f, 50.0f, 200.0f, 100.0f, { 0.82f, 0.22f, 0.22f, 1.0f });
			addRect(300.0f, 300.0f, 400.0f, 400.0f, { 0.82f, 0.82f, 0.22f, 1.0f });

			uint32 dataSize = (uint8*)vertexPtr - (uint8*)vertices;

			m_VertexBuffer = VertexBuffer::Create(vertices, dataSize);

			delete[] vertices;

			uint32* indices = new uint32[maxQuadIndices];

			uint32 offset = 0;
			for (uint32 i = 0; i < maxQuadIndices; i += 6)
			{
				indices[i + 0] = offset + 0;
				indices[i + 1] = offset + 1;
				indices[i + 2] = offset + 2;

				indices[i + 3] = offset + 2;
				indices[i + 4] = offset + 3;
				indices[i + 5] = offset + 0;

				offset += 4;
			}

			m_IndexBuffer = IndexBuffer::Create(indices, sizeof(uint32) * m_QuadIndexCount);

			delete[] indices;
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

					glm::mat4 projectionMatrix = glm::ortho(0.0f, (float)m_SwapchainFramebuffer->GetWidth(), 0.0f, (float)m_SwapchainFramebuffer->GetHeight());
					FLUX_SUBMIT_RENDER_COMMAND([commandBuffer = m_SwapchainCommandBuffer, pipeline = m_Pipeline, projectionMatrix]()
					{
						pipeline->RT_SetPushConstant(commandBuffer, ShaderStage::Vertex, &(projectionMatrix[0].x), 64);
					});

					m_Pipeline->DrawIndexed(m_SwapchainCommandBuffer, m_QuadIndexCount);

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