#pragma once

#include "CommandBuffer.h"
#include "GraphicsPipeline.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Texture.h"

namespace Flux {

	class RenderPipeline : public ReferenceCounted
	{
	public:
		struct CameraSettings
		{
			glm::mat4 ViewMatrix = glm::mat4(1.0f);
			glm::mat4 ProjectionMatrix = glm::mat4(1.0f);
			float NearClip = 0.01f;
			float FarClip = 1000.0f;
		};

		struct EnvironmentSettings
		{
			// TODO:
			// - Directional lights
			// - Point lights
			// - Skybox (LOD, intensity, etc)
		};
	public:
		virtual ~RenderPipeline() {}

		virtual void BeginRendering() = 0;
		virtual void EndRendering() = 0;

		virtual void BeginRendering2D() = 0;
		virtual void EndRendering2D() = 0;

		virtual void DrawQuad(const glm::mat4& transform, const glm::vec4& color = glm::vec4(1.0f)) = 0;
		virtual void DrawQuad(const glm::vec3& position, float rotation, const glm::vec2& scale, const glm::vec4& color = glm::vec4(1.0f)) = 0;
		virtual void DrawQuad(const glm::vec3& position, const glm::vec2& scale, const glm::vec4& color = glm::vec4(1.0f)) = 0;

		virtual void DrawQuad(const glm::mat4& transform, Ref<Texture2D> texture, const glm::vec4& color = glm::vec4(1.0f)) = 0;
		virtual void DrawQuad(const glm::vec3& position, float rotation, const glm::vec2& scale, Ref<Texture2D> texture, const glm::vec4& color = glm::vec4(1.0f)) = 0;
		virtual void DrawQuad(const glm::vec3& position, const glm::vec2& scale, Ref<Texture2D> texture, const glm::vec4& color = glm::vec4(1.0f)) = 0;

		virtual void SetViewportSize(uint32 width, uint32 height) = 0;

		virtual CameraSettings& GetCameraSettings() = 0;
		virtual const CameraSettings& GetCameraSettings() const = 0;

		virtual EnvironmentSettings& GetEnvironmentSettings() = 0;
		virtual const EnvironmentSettings& GetEnvironmentSettings() const = 0;
	};

	class ForwardRenderPipeline : public RenderPipeline
	{
	public:
		ForwardRenderPipeline();
		virtual ~ForwardRenderPipeline();

		virtual void BeginRendering() override;
		virtual void EndRendering() override;

		virtual void BeginRendering2D() override;
		virtual void EndRendering2D() override;

		virtual void DrawQuad(const glm::mat4& transform, const glm::vec4& color = glm::vec4(1.0f)) override;
		virtual void DrawQuad(const glm::vec3& position, float rotation, const glm::vec2& scale, const glm::vec4& color = glm::vec4(1.0f)) override;
		virtual void DrawQuad(const glm::vec3& position, const glm::vec2& scale, const glm::vec4& color = glm::vec4(1.0f)) override;
		
		virtual void DrawQuad(const glm::mat4& transform, Ref<Texture2D> texture, const glm::vec4& color = glm::vec4(1.0f)) override;
		virtual void DrawQuad(const glm::vec3& position, float rotation, const glm::vec2& scale, Ref<Texture2D> texture, const glm::vec4& color = glm::vec4(1.0f)) override;
		virtual void DrawQuad(const glm::vec3& position, const glm::vec2& scale, Ref<Texture2D> texture, const glm::vec4& color = glm::vec4(1.0f)) override;

		virtual void SetViewportSize(uint32 width, uint32 height) override;

		virtual CameraSettings& GetCameraSettings() override { return m_CameraSettings; }
		virtual const CameraSettings& GetCameraSettings() const override { return m_CameraSettings; }

		virtual EnvironmentSettings& GetEnvironmentSettings() override { return m_EnvironmentSettings; }
		virtual const EnvironmentSettings& GetEnvironmentSettings() const override { return m_EnvironmentSettings; }
	private:
		uint32 m_ViewportWidth = 0;
		uint32 m_ViewportHeight = 0;

		CameraSettings m_CameraSettings;
		EnvironmentSettings m_EnvironmentSettings;

		Ref<CommandBuffer> m_CommandBuffer;

		struct QuadVertex
		{
			glm::vec3 Position;
			glm::vec4 Color;
			glm::vec2 TexCoord;
			float TextureIndex;
		};

		inline static constexpr uint32 s_MaxQuads = 20000;
		inline static constexpr uint32 s_MaxQuadVertices = s_MaxQuads * 4;
		inline static constexpr uint32 s_MaxQuadIndices = s_MaxQuads * 6;
		inline static constexpr uint32 s_MaxTextureSlots = 32;

		Ref<GraphicsPipeline> m_QuadPipeline;
		Ref<Shader> m_QuadShader;
		std::vector<Ref<VertexBuffer>> m_QuadVertexBuffer;
		std::vector<QuadVertex*> m_QuadVertexStorage;
		QuadVertex* m_QuadVertexPointer = nullptr;

		std::array<Ref<Texture2D>, s_MaxTextureSlots> m_QuadTextureSlots;
		uint32 m_QuadTextureSlotIndex = 1;

		Ref<IndexBuffer> m_QuadIndexBuffer;
		uint32 m_QuadIndexCount = 0;
	};

}