#include "FluxPCH.h"
#include "RuntimeEngine.h"

#include "Flux/Runtime/Renderer/Renderer.h"

#include <stb_image.h>

namespace Flux {

	RuntimeEngine::RuntimeEngine(const EngineCreateInfo& createInfo)
		: Engine(createInfo)
	{
	}

	RuntimeEngine::~RuntimeEngine()
	{
		FLUX_CHECK_IS_IN_EVENT_THREAD();
	}

	void RuntimeEngine::OnInit()
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		uint32 width = m_MainWindow->GetWidth();
		uint32 height = m_MainWindow->GetHeight();
		m_EditorCamera.SetViewportSize(width, height);

		m_Scene = Ref<Scene>::Create();
		m_RenderPipeline = Ref<ForwardRenderPipeline>::Create(true);
	}

	void RuntimeEngine::OnShutdown()
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		FLUX_VERIFY(m_Scene->GetReferenceCount() == 1);
		m_Scene = nullptr;

		FLUX_VERIFY(m_RenderPipeline->GetReferenceCount() == 1);
		m_RenderPipeline = nullptr;
	}

	void RuntimeEngine::OnUpdate()
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		// TODO
		float deltaTime = m_DeltaTime;
		m_EditorCamera.OnUpdate(deltaTime);

		m_Scene->OnUpdate();
		m_Scene->OnRender(m_RenderPipeline, m_EditorCamera.GetViewMatrix(), m_EditorCamera.GetProjectionMatrix());
	}

	void RuntimeEngine::OnImGuiRender()
	{
		ImGui::Begin("Debug");

		ImGui::Text("Camera Position: [%.2f, %.2f, %.2f]", m_EditorCamera.GetPosition().X, m_EditorCamera.GetPosition().Y, m_EditorCamera.GetPosition().Z);
		ImGui::Text("Camera Rotation: [%.2f, %.2f, %.2f]", m_EditorCamera.GetRotation().X, m_EditorCamera.GetRotation().Y, m_EditorCamera.GetRotation().Z);

		ImGui::Separator();

		BuildConfiguration buildConfig = Engine::GetBuildConfiguration();
		const char* buildConfigString = Utils::BuildConfigurationToString(buildConfig);
		ImGui::Text("%s build", buildConfigString);
		ImGui::Separator();

		ImGui::Text("Ticks per second: %d", m_TicksPerSecond);
		ImGui::Text("Events per second: %d", m_EventsPerSecond);
		ImGui::Separator();

		ImGui::Checkbox("V-Sync", &m_VSync);
		ImGui::Text("%d fps", m_FramesPerSecond);
		ImGui::Text("Delta Time: %.2fms", m_DeltaTime * 1000.0f);
		// ImGui::DragFloat("Fixed Delta Time", &m_FixedDeltaTime, 0.001f, 0.001f, 0.1f);
		ImGui::Text("Fixed Delta Time: %.2f", m_FixedDeltaTime);

		ImGui::Separator();
		ImGui::Text("Command queues: %d", Renderer::GetQueueCount());

		ImGui::BeginDisabled();
		bool multithreaded = m_RenderThread != nullptr;
		ImGui::Checkbox("Multithreaded", &multithreaded);
		ImGui::EndDisabled();

		if (m_RenderThread)
		{
			ImGui::Separator();
			ImGui::Text("Render Thread wait: %.2fms", m_RenderThreadWaitTime);
		}

		ImGui::End();
	}

	void RuntimeEngine::OnEvent(Event& event)
	{
		EventHandler handler(event);
		handler.Bind<WindowResizeEvent>(FLUX_BIND_CALLBACK(OnWindowResizeEvent, this));
	}

	void RuntimeEngine::OnWindowResizeEvent(WindowResizeEvent& event)
	{
		m_RenderPipeline->SetViewportSize(event.GetWidth(), event.GetHeight());
		m_EditorCamera.SetViewportSize(event.GetWidth(), event.GetHeight());
	}

}