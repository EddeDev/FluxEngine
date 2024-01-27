#pragma once

#include "Flux/Runtime/Core/BaseTypes.h"
#include "Flux/Runtime/Core/AssertionMacros.h"

namespace Flux {

	enum class GraphicsAPI : uint8
	{
		None = 0,
		OpenGL,
		DX11
	};

	namespace Utils {

		inline const char* GraphicsAPIToString(GraphicsAPI api)
		{
			switch (api)
			{
			case GraphicsAPI::OpenGL: return "OpenGL";
			case GraphicsAPI::DX11: return "DX11";
			}
			FLUX_ASSERT(false, "Unknown Graphics API.");
			return "";
		}

	}

}