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

		Ref<Mesh> m_ActiveMesh;
		Ref<Mesh> m_GunMesh;
		Ref<Mesh> m_CityMesh;
		Ref<Mesh> m_BackpackMesh;
		int32 m_NumVisibleSubmeshes = 0;
		bool m_UseNormalMaps = true;

		Vector3 m_LightColor = Vector3(0.9f, 0.87f, 0.96f);
		Vector3 m_LightPosition = Vector3(2000.0f, 2000.0f, 2000.0f);
		float m_AmbientMultiplier = 0.0f;

		EditorCamera m_EditorCamera;
	};

}