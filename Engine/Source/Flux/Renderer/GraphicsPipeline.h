#pragma once

#include "VertexDeclaration.h"

#include "IndexBuffer.h"

namespace Flux {

	enum class PrimitiveTopology : uint8
	{
		Triangles = 0
	};

	struct GraphicsPipelineCreateInfo
	{
		Flux::VertexDeclaration VertexDeclaration;

		PrimitiveTopology Topology = PrimitiveTopology::Triangles;
	};

	class GraphicsPipeline : public ReferenceCounted
	{
	public:
		virtual ~GraphicsPipeline() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void DrawIndexed(IndexBufferDataType dataType, uint32 indexCount, uint32 startIndexLocation = 0, uint32 baseVertexLocation = 0) const = 0;

		static Ref<GraphicsPipeline> Create(const GraphicsPipelineCreateInfo& createInfo);
	};

}