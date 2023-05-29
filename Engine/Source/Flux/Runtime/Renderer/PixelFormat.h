#pragma once

#include "Flux/Runtime/Core/BaseTypes.h"
#include "Flux/Runtime/Core/AssertionMacros.h"

namespace Flux {

	enum class PixelFormat : uint8
	{
		None = 0,

		RGBA,
		RGBA16F,
		RGBA32F,

		Depth24Stencil8,
		Depth32FStencil8UInt,
		Depth32F
	};

	namespace Utils {

		inline static bool IsDepthFormat(PixelFormat format)
		{
			return format == PixelFormat::Depth24Stencil8|| format == PixelFormat::Depth32FStencil8UInt || format == PixelFormat::Depth32F;
		}

		inline static bool IsStencilFormat(PixelFormat format)
		{
			return format == PixelFormat::Depth24Stencil8 || format == PixelFormat::Depth32FStencil8UInt;
		}

		inline static uint32 ComputeBytesPerPixel(PixelFormat format)
		{
			FLUX_ASSERT(!IsDepthFormat(format));
			switch (format)
			{
			case PixelFormat::RGBA:    return 4;
			case PixelFormat::RGBA16F: return 8;
			case PixelFormat::RGBA32F: return 16;
			}
			FLUX_VERIFY(false, "Unknown pixel format");
			return 0;
		}


	}

}