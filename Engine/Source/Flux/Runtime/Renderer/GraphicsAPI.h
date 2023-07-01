#pragma once

#include "Flux/Runtime/Core/BaseTypes.h"

namespace Flux {

	enum class GraphicsAPI : uint8
	{
		None = 0,
		Vulkan
	};

	namespace Utils {

		inline static const char* GraphicsAPIToString(GraphicsAPI api)
		{
			switch (api)
			{
			case GraphicsAPI::Vulkan: return "Vulkan";
			}
			FLUX_VERIFY(false, "Unknown Graphics API.");
			return "";
		}

	}

}