#pragma once

#include "Flux/Core/BaseTypes.h"
#include "Flux/Core/AssertionMacros.h"

namespace Flux {

	enum class GraphicsAPI : uint8
	{
		None = 0,
		OpenGL
	};

	namespace Utils {

		inline const char* GraphicsAPIToString(GraphicsAPI api)
		{
			switch (api)
			{
			case GraphicsAPI::OpenGL: return "OpenGL";
			}
			FLUX_ASSERT(false, "Unknown Graphics API.");
			return "";
		}

	}

}