#pragma once

#include "Engine.h"

#include "Flux/Runtime/Scene/Scene.h"
#include "Flux/Runtime/Scene/Entity.h"
#include "Flux/Runtime/Renderer/RenderPipeline.h"

// TODO: Temp
#include "Flux/Editor/Project/Project.h"
#include "Flux/Editor/EditorAssetDatabase.h"

namespace Flux {

	class RuntimeEngine : public Engine
	{
	public:
		RuntimeEngine(const EngineCreateInfo& createInfo);
		virtual ~RuntimeEngine();
	protected:
		virtual void OnInit() override;
		virtual void OnShutdown() override;
		virtual void OnUpdate() override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Event& event) override;
	private:
		void OnWindowResizeEvent(WindowResizeEvent& event);
	private:
		Ref<Project> m_Project;
		Ref<Scene> m_Scene;
		Ref<RenderPipeline> m_RenderPipeline;
	};

}