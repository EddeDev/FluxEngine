#pragma once

#include "Flux/Runtime/Core/BaseTypes.h"

namespace Flux {

	struct IntRect
	{
		int32 MinX;
		int32 MinY;
		int32 MaxX;
		int32 MaxY;

		IntRect()
			: MinX(0), MinY(0), MaxX(0), MaxY(0) {}
		IntRect(int32 minX, int32 minY, int32 maxX, int32 maxY)
			: MinX(minX), MinY(minY), MaxX(maxX), MaxY(maxY) {}

		int32 GetArea() const { return (MaxX - MinX) * (MaxY - MinY); }

		int32 GetWidth() const { return MaxX - MinX; }
		int32 GetHeight() const { return MaxY - MinY; }
	};

}