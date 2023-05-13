#pragma once

#include "Flux/Runtime/Renderer/GraphicsContext.h"

namespace Flux {

	class VulkanContext : public GraphicsContext
	{
	public:
		VulkanContext();
		virtual ~VulkanContext();
	};

}