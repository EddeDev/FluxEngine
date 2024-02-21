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
		void RenderMesh(Ref<Mesh> mesh, const Matrix4x4& transform);
		void RenderMeshWithMaterial(Ref<Mesh> mesh, const MaterialDescriptor& material, const Matrix4x4& transform);

		void OnWindowResizeEvent(WindowResizeEvent& event);
	private:
		Ref<Shader> m_Shader;
		Ref<GraphicsPipeline> m_Pipeline;
		Ref<Framebuffer> m_Framebuffer;

		MaterialDescriptor m_Material;
		Ref<Mesh> m_SphereMesh;

		Ref<Texture> m_CubemapTexture;

		Ref<Texture> m_WhiteTexture;
		Ref<Texture> m_CheckerboardTexture;

		Vector3 m_LightColor = Vector3(0.9f, 0.87f, 0.96f);
		Vector3 m_LightRotation = Vector3(50.0f, -30.0f, 0.0f);
		float m_AmbientMultiplier = 0.0f;

		EditorCamera m_EditorCamera;
	};

}