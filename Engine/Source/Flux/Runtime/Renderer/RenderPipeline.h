#pragma once

#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Framebuffer.h"

namespace Flux {

	struct DynamicMeshSubmitInfo
	{
		Ref<Mesh> Mesh;
		uint32 SubmeshIndex;
		Matrix4x4 Transform;
	};

	struct StaticMeshSubmitInfo
	{
		Ref<Mesh> Mesh;
		Matrix4x4 Transform;
	};

	class RenderPipeline : public ReferenceCounted
	{
	public:
		struct CameraSettings
		{
			Matrix4x4 ViewMatrix = Matrix4x4(1.0f);
			Matrix4x4 ProjectionMatrix = Matrix4x4(1.0f);
			float NearClip = 0.1f;
			float FarClip = 1000.0f;
		};

		struct EnvironmentSettings
		{
			Vector3 LightDirection = Vector3(0.0f);
			Vector3 LightColor = Vector3(1.0f);

			// TODO:
			// - Directional lights
			// - Point lights
			// - Skybox (LOD, intensity)
		};
	public:
		virtual void BeginRendering() = 0;
		virtual void EndRendering() = 0;
		virtual void SubmitDynamicMesh(const DynamicMeshSubmitInfo& submitInfo) = 0;
		virtual void SubmitStaticMesh(const StaticMeshSubmitInfo& submitInfo) = 0;

		virtual void BeginRendering2D() = 0;
		virtual void EndRendering2D() = 0;

		virtual void SetViewportSize(uint32 width, uint32 height) = 0;

		virtual uint32 GetViewportWidth() const = 0;
		virtual uint32 GetViewportHeight() const = 0;

		virtual Ref<Texture> GetComposedTexture() const = 0;

		virtual CameraSettings& GetCameraSettings() = 0;
		virtual const CameraSettings& GetCameraSettings() const = 0;

		virtual EnvironmentSettings& GetEnvironmentSettings() = 0;
		virtual const EnvironmentSettings& GetEnvironmentSettings() const = 0;
	};

	class ForwardRenderPipeline : public RenderPipeline
	{
	public:
		ForwardRenderPipeline(bool swapchainTarget = false);
		virtual ~ForwardRenderPipeline();

		virtual void BeginRendering() override;
		virtual void EndRendering() override;
		virtual void SubmitDynamicMesh(const DynamicMeshSubmitInfo& submitInfo) override;
		virtual void SubmitStaticMesh(const StaticMeshSubmitInfo& submitInfo) override;

		virtual void BeginRendering2D() override;
		virtual void EndRendering2D() override;

		virtual void SetViewportSize(uint32 width, uint32 height) override;

		virtual uint32 GetViewportWidth() const override { return m_ViewportWidth; }
		virtual uint32 GetViewportHeight() const override { return m_ViewportHeight; }

		virtual Ref<Texture> GetComposedTexture() const override { return m_Framebuffer->GetColorAttachment(); }

		virtual CameraSettings& GetCameraSettings() override { return m_CameraSettings; }
		virtual const CameraSettings& GetCameraSettings() const override { return m_CameraSettings; }

		virtual EnvironmentSettings& GetEnvironmentSettings() override { return m_EnvironmentSettings; }
		virtual const EnvironmentSettings& GetEnvironmentSettings() const override { return m_EnvironmentSettings; }
	private:
		uint32 m_ViewportWidth = 0;
		uint32 m_ViewportHeight = 0;

		CameraSettings m_CameraSettings;
		EnvironmentSettings m_EnvironmentSettings;

		Ref<Shader> m_Shader;
		Ref<GraphicsPipeline> m_Pipeline;
		Ref<Framebuffer> m_Framebuffer;
		Ref<Texture> m_WhiteTexture;
		MaterialDescriptor m_Material;

		float m_AmbientMultiplier = 0.0f;

		struct DrawCommand
		{
			Ref<Mesh> Mesh;
			uint32 SubmeshIndex;
			Matrix4x4 Transform;
		};

		std::vector<DrawCommand> m_DrawCommandQueue;
	};

}