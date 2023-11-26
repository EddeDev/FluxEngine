#pragma once

#include "Flux/Renderer/GraphicsPipeline.h"

namespace Flux {

	class OpenGLPipeline : public GraphicsPipeline
	{
	public:
		OpenGLPipeline(const GraphicsPipelineCreateInfo& createInfo);
		virtual ~OpenGLPipeline();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void Scissor(int32 x, int32 y, int32 width, int32 height) const override;

		virtual void DrawIndexed(IndexBufferDataType dataType, uint32 indexCount, uint32 startIndexLocation = 0, uint32 baseVertexLocation = 0) const override;
	private:
		PrimitiveTopology m_Topology = PrimitiveTopology::None;
	
		struct OpenGLPipelineData
		{
			Flux::VertexDeclaration VertexDeclaration;
			uint32 VertexArrayID = 0;
		};

		OpenGLPipelineData* m_Data = nullptr;
	};

}