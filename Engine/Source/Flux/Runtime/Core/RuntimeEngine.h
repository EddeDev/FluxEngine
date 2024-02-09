#pragma once

#include "Engine.h"

#include "Flux/Runtime/Renderer/Shader.h"
#include "Flux/Runtime/Renderer/GraphicsPipeline.h"

#include "Flux/Runtime/Renderer/Mesh.h"

// TODO: Temp
#include "Flux/Editor/EditorCamera.h"

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
		Ref<Shader> m_Shader;
		Ref<GraphicsPipeline> m_Pipeline;

		Ref<Mesh> m_Mesh;
		int32 m_NumVisibleSubmeshes = 0;

		EditorCamera m_EditorCamera;
	};

}