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
	private:
		GraphicsPipelineCreateInfo m_CreateInfo;

		uint32 m_VertexArrayID = 0;
	};

}