#include "FluxPCH.h"
#include "RenderPipeline.h"

#include "Flux/Runtime/Core/Engine.h"

namespace Flux {

	ForwardRenderPipeline::ForwardRenderPipeline(bool swapchainTarget)
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		m_ViewportWidth = Engine::Get().GetMainWindow()->GetWidth();
		m_ViewportHeight = Engine::Get().GetMainWindow()->GetHeight();

		m_Shader = Shader::Create("Resources/Shaders/Shader.glsl");

		GraphicsPipelineCreateInfo pipelineCreateInfo;
		pipelineCreateInfo.VertexDeclaration = {
			{ "a_Position", VertexElementFormat::Float3 },
			{ "a_Normal", VertexElementFormat::Float3 },
			{ "a_Tangent", VertexElementFormat::Float3 },
			{ "a_Binormal", VertexElementFormat::Float3 },
			{ "a_TexCoord", VertexElementFormat::Float2 }
		};
		pipelineCreateInfo.DepthTest = true;
		pipelineCreateInfo.DepthWrite = true;
		pipelineCreateInfo.BackfaceCulling = true;
		m_Pipeline = GraphicsPipeline::Create(pipelineCreateInfo);

		FramebufferCreateInfo framebufferCreateInfo;
		framebufferCreateInfo.Attachments = { TextureFormat::RGBA32, TextureFormat::Depth24Stencil8 };
		framebufferCreateInfo.DepthCompareFunction = CompareFunction::GreaterOrEqual;
		framebufferCreateInfo.SwapchainTarget = swapchainTarget;
		m_Framebuffer = Framebuffer::Create(framebufferCreateInfo);

		// White texture
		{
			uint32 whiteTextureData = 0xFFFFFFFF;

			TextureProperties properties;
			properties.Width = 1;
			properties.Height = 1;
			properties.Format = TextureFormat::RGBA32;
			m_WhiteTexture = Texture::Create(properties, &whiteTextureData);
		}

		// Default material
		{
			m_Material.AlbedoMap = m_WhiteTexture;
			m_Material.NormalMap = m_WhiteTexture;
			m_Material.RoughnessMap = m_WhiteTexture;
			m_Material.MetalnessMap = m_WhiteTexture;

			m_Material.AlbedoColor = Vector4(1.0f);
			m_Material.Metalness = 0.0f;
			m_Material.Roughness = 0.5f;
			m_Material.Emission = 0.0f;
			m_Material.Name = "Default Material";
		}
	}

	ForwardRenderPipeline::~ForwardRenderPipeline()
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();
	}

	void ForwardRenderPipeline::BeginRendering()
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();
	}

	void ForwardRenderPipeline::EndRendering()
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		m_Framebuffer->Bind();

		m_Shader->Bind();
		m_Shader->SetUniform("u_LightColor", m_EnvironmentSettings.LightColor);
		m_Shader->SetUniform("u_AmbientMultiplier", m_AmbientMultiplier);
		m_Shader->SetUniform("u_ViewMatrix", m_CameraSettings.ViewMatrix);
		m_Shader->SetUniform("u_ViewProjectionMatrix", m_CameraSettings.ViewProjectionMatrix);
		m_Shader->SetUniform("u_CameraPosition", m_CameraSettings.CameraPosition);
		m_Shader->SetUniform("u_LightDirection", m_EnvironmentSettings.LightDirection);

		for (auto it = m_DrawCommandQueue.begin(); it != m_DrawCommandQueue.end(); it++)
		{
			it->Mesh->GetVertexBuffer()->Bind();
			m_Pipeline->Bind();
			m_Pipeline->Scissor(0, 0, m_ViewportWidth, m_ViewportHeight);
			it->Mesh->GetIndexBuffer()->Bind();

			m_Shader->Bind();

			auto& properties = it->Mesh->GetProperties();
			auto& submesh = properties.Submeshes[it->SubmeshIndex];

			m_Shader->SetUniform("u_Transform", it->Transform);

			// TODO: replace
			auto& material = m_Material;

			m_Shader->SetUniform("u_AlbedoColor", material.AlbedoColor);
			m_Shader->SetUniform("u_Roughness", material.Roughness);
			m_Shader->SetUniform("u_Metalness", material.Metalness);
			m_Shader->SetUniform("u_Emission", material.Emission);

			bool hasNormalMap = !material.NormalMap.Equals(m_WhiteTexture);
			m_Shader->SetUniform("u_HasNormalMap", uint32(hasNormalMap ? 1 : 0));

			if (material.AlbedoMap)
			{
				material.AlbedoMap->Bind(0);
				m_Shader->SetUniform("u_AlbedoMap", 0);
			}

			if (material.NormalMap)
			{
				material.NormalMap->Bind(1);
				m_Shader->SetUniform("u_NormalMap", 1);
			}

			if (material.RoughnessMap)
			{
				material.RoughnessMap->Bind(2);
				m_Shader->SetUniform("u_RoughnessMap", 2);
			}

			if (material.MetalnessMap)
			{
				material.MetalnessMap->Bind(3);
				m_Shader->SetUniform("u_MetalnessMap", 3);
			}

			m_Pipeline->DrawIndexed(
				submesh.IndexFormat,
				submesh.IndexCount,
				submesh.StartIndexLocation,
				submesh.BaseVertexLocation
			);

			if (material.AlbedoMap)
				material.AlbedoMap->Unbind(0);
			if (material.NormalMap)
				material.NormalMap->Unbind(1);
			if (material.RoughnessMap)
				material.RoughnessMap->Unbind(2);
			if (material.MetalnessMap)
				material.MetalnessMap->Unbind(3);
		}
		m_Framebuffer->Unbind();

		m_DrawCommandQueue.clear();
	}

	void ForwardRenderPipeline::SubmitDynamicMesh(const DynamicMeshSubmitInfo& submitInfo)
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		auto& drawCommand = m_DrawCommandQueue.emplace_back();
		drawCommand.Mesh = submitInfo.Mesh;
		drawCommand.SubmeshIndex = submitInfo.SubmeshIndex;
		drawCommand.Transform = submitInfo.Transform;
	}

	void ForwardRenderPipeline::SubmitStaticMesh(const StaticMeshSubmitInfo& submitInfo)
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		auto& properties = submitInfo.Mesh->GetProperties();
		for (uint32 i = 0; i < (uint32)properties.Submeshes.size(); i++)
		{
			auto& drawCommand = m_DrawCommandQueue.emplace_back();
			drawCommand.Mesh = submitInfo.Mesh;
			drawCommand.SubmeshIndex = i;
			drawCommand.Transform = submitInfo.Transform * properties.Submeshes[i].WorldTransform;
		}
	}

	void ForwardRenderPipeline::BeginRendering2D()
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();
	}

	void ForwardRenderPipeline::EndRendering2D()
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();
	}

	void ForwardRenderPipeline::SetViewportSize(uint32 width, uint32 height)
	{
		FLUX_CHECK_IS_IN_MAIN_THREAD();

		if (m_ViewportWidth != width || m_ViewportHeight != height)
		{
			m_ViewportWidth = width;
			m_ViewportHeight = height;

			m_Framebuffer->Resize(width, height);
		}
	}

}