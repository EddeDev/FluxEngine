#include "FluxPCH.h"
#include "GraphicsPipeline.h"

#include "Flux/Core/Engine.h"

#include "OpenGL/OpenGLPipeline.h"

namespace Flux {

	Ref<GraphicsPipeline> GraphicsPipeline::Create(const GraphicsPipelineCreateInfo& createInfo)
	{
		switch (Engine::Get().GetGraphicsAPI())
		{
		case GraphicsAPI::OpenGL: return Ref<OpenGLPipeline>::Create(createInfo);
		}
		FLUX_ASSERT(false, "Unknown Graphics API.");
		return nullptr;
	}

}