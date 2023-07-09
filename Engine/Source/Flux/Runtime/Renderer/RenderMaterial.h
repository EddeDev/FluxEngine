#pragma once

#include "GraphicsPipeline.h"
#include "Shader.h"
#include "Image.h"
#include "CommandBuffer.h"

namespace Flux {

	class RenderMaterial : public ReferenceCounted
	{
	public:
		virtual ~RenderMaterial() {}

		virtual void Bind(Ref<CommandBuffer> commandBuffer, Ref<GraphicsPipeline> pipeline) = 0;
		virtual void RT_Bind(Ref<CommandBuffer> commandBuffer, Ref<GraphicsPipeline> pipeline) = 0;

		virtual void Set(std::string_view name, float value) = 0;
		virtual void Set(std::string_view name, const glm::vec2& value) = 0;
		virtual void Set(std::string_view name, const glm::vec3& value) = 0;
		virtual void Set(std::string_view name, const glm::vec4& value) = 0;

		virtual void Set(std::string_view name, Ref<Image2D> image) = 0;

		static Ref<RenderMaterial> Create(Ref<Shader> shader, const std::string& name = "");
		static Ref<RenderMaterial> Create(Ref<RenderMaterial> other, const std::string& name = "");
	};

}