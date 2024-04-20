#include "FluxPCH.h"
#include "RuntimeEngine.h"

#include "Flux/Runtime/Renderer/Renderer.h"

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

		// TODO: very temp
		m_Project = Project::LoadFromFile("C:/Users/Edvin Pettersson/Desktop/FluxProject");
		m_Project->RegisterAssetDatabase<EditorAssetDatabase>();

		m_Scene = Ref<Scene>::Create();
		m_RenderPipeline = Ref<ForwardRenderPipeline>::Create(true);

		{
			Entity cubeEntity = m_Scene->CreateEmpty("Cube");
			cubeEntity.GetComponent<TransformComponent>().SetLocalPosition({ -4.0f, 0.0f, 0.0f });
			Ref<Mesh> cubeMeshAsset = m_Project->GetAssetDatabase()->GetAssetFromPath("Assets/Meshes/Primitives/Cube.gltf");
			cubeEntity.AddComponent<SubmeshComponent>(cubeMeshAsset->GetAssetID());
			cubeEntity.AddComponent<MeshRendererComponent>();

			Entity sphereEntity = m_Scene->CreateEmpty("Sphere");
			sphereEntity.GetComponent<TransformComponent>().SetLocalPosition({ 0.0f, 0.0f, 0.0f });
			Ref<Mesh> sphereMeshAsset = m_Project->GetAssetDatabase()->GetAssetFromPath("Assets/Meshes/Primitives/Sphere.gltf");
			sphereEntity.AddComponent<SubmeshComponent>(sphereMeshAsset->GetAssetID());
			sphereEntity.AddComponent<MeshRendererComponent>();

			Entity cameraEntity = m_Scene->CreateCamera("Main Camera");
			cameraEntity.GetComponent<TransformComponent>().SetLocalPosition({ 0.0f, 1.0f, -10.0f });

			Entity directionalLightEntity = m_Scene->CreateDirectionalLight("Directional Light", { 50.0f, -30.0f, 0.0f });
		}
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

		m_Scene->OnUpdate();
		m_Scene->OnRender(m_RenderPipeline);
	}

	void RuntimeEngine::OnImGuiRender()
	{
		ImGui::Begin("Debug");

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

#ifdef FLUX_MATH_DEBUG_ENABLED
		ImGui::Separator();

		auto& mathFunctionCalls = MathDebug::GetFunctionCalls();

		for (auto& [functionName, numCalls] : mathFunctionCalls)
		{
			std::string textString = fmt::format("{0}: {1} calls", functionName, numCalls);
			ImGui::TextUnformatted(textString.c_str());
		}
#endif

		ImGui::End();
	}

	void RuntimeEngine::OnEvent(Event& event)
	{
		EventHandler handler(event);
		handler.Bind<WindowResizeEvent>(FLUX_BIND_CALLBACK(OnWindowResizeEvent, this));
	}

	void RuntimeEngine::OnWindowResizeEvent(WindowResizeEvent& event)
	{
		m_Scene->SetViewportSize(event.GetWidth(), event.GetHeight());
		m_RenderPipeline->SetViewportSize(event.GetWidth(), event.GetHeight());
	}

}