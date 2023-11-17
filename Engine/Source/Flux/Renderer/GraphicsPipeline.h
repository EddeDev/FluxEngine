#pragma once

#include "VertexDeclaration.h"

namespace Flux {

	struct GraphicsPipelineCreateInfo
	{
		Flux::VertexDeclaration VertexDeclaration;
	};

	class GraphicsPipeline : public ReferenceCounted
	{
	public:
		virtual ~GraphicsPipeline() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		static Ref<GraphicsPipeline> Create(const GraphicsPipelineCreateInfo& createInfo);
	};

}